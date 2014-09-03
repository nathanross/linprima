
#ifdef LOWMEM


int JsonDecompressor::getDecodeIdx(char in) {
    if (in >= '0' && in <= '9') {
        return in - '0';
    } else if (in >= 'A' && in <= 'Z') {
        return (in - 'A') + 10;
    } else if (in >= 'a' && in <= 'z') {
        return (in - 'a') + 36;
    } else {
        return -1;
    }
}

JsonDecompressor::JsonDecompressor(
#ifdef LIMITJSON
                                   vector<string> * completeObjs,
#endif
                                   long lenArg)
#ifdef LIMITJSON
    : completeObjects(completeObjs) 
#endif
{
    len = lenArg+50;//+50 for a basic floor.
    //ideally, we want most calls to fit within one block.
    //because there's way more allocated mem
    //to going over a little bit (and way more alloc calls
    // for setting block size low) then by going
    // under by half.
    blockSize = (len) *90; 
    if (blockSize > MAX_BLOCK_SIZE) {
        blockSize = MAX_BLOCK_SIZE;
    }
    //printf("received input length of %li , calculated blockSize of %li \n", len,blockSize);
    current = 0x0;
    i = blockSize;    
}
void JsonDecompressor::Put(char in) {
#ifndef LIMITJSON
    if (i == blockSize) {
        current = (char *) malloc(blockSize);
        blocks.push_back(current);
        i = 0;
    }
    current[i] = in;
    ++i;    
#endif
#ifdef LIMITJSON
    //printf("\n%c|",in);
    putStack.push_back(&in);
    putStackLen.push_back(1);
    putStackPos.push_back(0);
    char c = in;

    //review:
    //current: current block
    //i: current position in block
    //c: current character in source document or expanded string
    //    expanded string is read right after quotation following
    //    addr is read.
    //putStack: stack of expanded strings.
    int inpos=0;
    while (! putStack.empty()) {
        if (i == blockSize) {
            current = (char *) malloc(blockSize);
            blocks.push_back(current);
            i = 0;
        }
        if (inpos == putStackLen.back()) {
            putStack.pop_back();
            putStackLen.pop_back();
            putStackPos.pop_back();
            if (putStack.empty()) {
                return;
            }
            inpos = putStackPos.back();
            //printf(" stackframe complete adding end bracket }\n");
            //current[i] = '}';
            //++i;
            continue;
        }
        //printf("\ndepth %i pos %i out of len %i \n", (int) putStackPos.size(), (int) inpos, (int) putStackLen.back());
        c = putStack.back()[inpos];
        //printf(" -%c-",c);

        //printf("seq %i ", (int) objExpandSeq);
        if (objExpandSeq == OBJ_NONE && 
            c != '[' &&
            c != ',' &&
            c != ':') {

            current[i] = c;
            ++i;
            ++inpos;
            continue;
        } else if (objExpandSeq == OBJ_NONE) {
            //printf(" found init  ");
            objExpandSeq = OBJ_MARKER_BEGIN;
        } else if (objExpandSeq >= OBJ_MARKER_BEGIN &&
                   objExpandSeq <= OBJ_MARKER_END) {
            if (c == MARKER[objExpandSeq-OBJ_MARKER_BEGIN]) {
                //printf(" found marker part ");
                ++objExpandSeq;
                if (objExpandSeq > OBJ_MARKER_END) {
                    addr = 0;
                    objExpandSeq = OBJ_GET_ADDR;
                }
                ++(inpos);
                continue;
            } else {  
                //                printf(" sequence unfinished ");
                for (int m=0;m<objExpandSeq-OBJ_MARKER_BEGIN;m++){
                    //printf("adding back %c", MARKER[m]);
                    current[i] = MARKER[m];
                    ++i;
                    if (i == blockSize) {
                        current = (char *) malloc(blockSize);
                        blocks.push_back(current);
                        i = 0;
                    }
                }
                objExpandSeq = OBJ_NONE;
                continue;
            } 
        } else if (objExpandSeq == OBJ_GET_ADDR) {
            if (c != '"') {
                addr *= 62;
                addr += getDecodeIdx(c);
                //printf(" getting addr... ");
                ++(inpos);
                continue;
            } else {
                ++(inpos);
                putStackPos.pop_back();
                putStackPos.push_back(inpos);
                //printf(" seeking addr %i ", (int) addr);
                //printf("at completeObjs of size %i \n", 
                       //       (int) completeObjects->size());
                putStack.push_back(completeObjects->at(addr).data());
                putStackLen.push_back(completeObjects->at(addr).length());
                putStackPos.push_back(0);
                inpos = 0;
                objExpandSeq = OBJ_NONE;
                continue;
            }
        }

        current[i] = c;
        ++i;    
        ++(inpos);
    }
#endif
}

void JsonDecompressor::decompress(char *&decoded, long &lenOut) {
    int lastBlock = blocks.size()-1;
    long lastBlockSize = i;
    //blocks[lastidx] = (char *) realloc(lastidx, i); 

    long ALLOC_CEILING = 20000; 
    /*
      THRESHOLD: maximum # of bytes to alloc or realloc at a time. 
      that is, the amount of memory that will at sometimes be 
      allocated in rough excess to the amount allocated by the 
      decompressed string. This const is a balancing act between 
      reducing the maximum possible heap peak for large inputs, 
      and the performance cost with large inputs of frequent 
      reallocations of  large (or very frequent reallocations 
      of small) amounts of memory. */

    const long HIGH_COMP_FACTOR = 8; 
    /* 
       if the string is much smaller than the allocation ceiling,
       then by guessing a factor which it will probably not expand
       past we can reduce the amount of memory allocated

       effectively this brings down the allocation ceiling 
       (though never raise it) so if HCF is 8, and the 
       compressed string passed in is 1000 chars, the new effective
       alloc ceiling for that input is 8000 chars. 

       This const is a balancing act between the cost of 
       unnecessary reallocation (if for many cases you 
       guess too low) and the performance cost of unnecessary
       allocation and free'ing (if you guess too high)  */
    if (HIGH_COMP_FACTOR * len < ALLOC_CEILING) {
        ALLOC_CEILING = HIGH_COMP_FACTOR * len;
    }

    long curSize = 0;
    long iOut =0;

    //idx here refers to the index on the product (reversed) array.
    //char *decoded;

    //should be at least equal to the longest char length a sequence
    //may be replaced by.
    long longestReplacement = 20;
    long iOutStopAt = -1; 

    int sequenceStep = 0;
    const int NONE = 0;
    const int FOUND_INIT = 1;
    const int FOUND_QUOTE = 2;
    const int FIRST_CHAR_ATE = 3;

    int firstDecodeIdx;
    int decodeIdx = -1;
    const char * decodedPtr;
    bool firstAdd = true;
    int ignoreNextStrval = 0;

    long curBlock = 0;
    long curBlockSize = (lastBlock ==0)? lastBlockSize : blockSize;
    char *block = blocks[0];
    char lastChar;
    long b =0;    
    //printf("\n\n%s", string(blocks[0], curBlockSize-1).data());
    while (curBlock <= lastBlock) {
        if (iOut >= iOutStopAt) {
            curSize += ALLOC_CEILING;
            iOutStopAt = curSize - longestReplacement;
            if (firstAdd) { 
                decoded = (char*) malloc(curSize); 
                firstAdd = false;
            } else {
                //we've used up the threshold, so we 
                //allocate more memory (while reducing the size of the
                // original string)
                decoded = (char*) realloc(decoded, curSize); 
            }            
        }
        // within the inner while(...) loop:
        // 1.
        // never depend on the block for access to a previous 
        // value of the block (instead store the value if it
        // may be relevant later) as any values may
        // be deallocated by the next char. in other words,
        // never decrement b.
        // 
        // 2.
        // only access the character at the current position
        // if you increment or change the position (b),
        // wait for the next loop (so that block[b] is available
        // even if it's in the next block) because we want
        // to read every character (and can't go back with b)
        // never increment b more than once per loop.
        while (iOut < iOutStopAt && b < curBlockSize) {
            //printf("\n %i %c ", (int) iOut, block[b]);
            if (sequenceStep == NONE) {
                if (block[b] != ':' 
                    && block[b] != ',' 
                    && block[b] != '{'
                    && block[b] != '[') {
                    //although redundant with the 'default case' 
                    // at the bottom which several
                    //consequents below fall to, because most 
                    // characters won't be part of a sequence
                    //we want to check if it's a character 
                    //like that first to reduce the number of
                    //tests/jumps.

                    decoded[iOut] = block[b];
                    ++iOut;
                    ++b;
                    continue;
                } else {
                    if (ignoreNextStrval == 0 ||
                        block[b] != ':') {
                        //printf("found init");
                        sequenceStep = FOUND_INIT;
                    }
                    if (ignoreNextStrval > 0) {
                        ignoreNextStrval--;
                    }
                }
            } else if (sequenceStep == FOUND_INIT) {
                if (block[b] == '"') {
                    sequenceStep = FOUND_QUOTE;
                } else {
                    sequenceStep = NONE;
                    continue; //check for a new init here.
                }
            } else if (sequenceStep == FOUND_QUOTE) {
                //"eating first char";               
                firstDecodeIdx = getDecodeIdx(block[b]);
                if (firstDecodeIdx >= 0) {
                    //it's a valid char.
                    lastChar = block[b];
                    sequenceStep = FIRST_CHAR_ATE;
                    ++b;
                    continue;
                } else {
                    //not part of a sequence or empty str. no big deal.
                    sequenceStep = NONE;
                    continue; //check for a new init here.
                }
            } else if (sequenceStep == FIRST_CHAR_ATE) {
                //looking at second char;
                sequenceStep = NONE;
                decodeIdx = getDecodeIdx(block[b]);
                //if returned decodeIdx is -1, gets the 
                //first ptr in the array 
                // (the one for a single character)
                decodedPtr = text::decoder
                    [firstDecodeIdx][decodeIdx +1];

                if (decodedPtr !=0x0) { 
                    //printf("expanding: %s ", decodedPtr);
                    if (decodedPtr == text::_raw_FULL
                        || decodedPtr == text::_name_FULL
                        || decodedPtr == text::_operator_FULL
                        || decodedPtr == text::_description_FULL
                        || decodedPtr == text::_message_FULL
                        || decodedPtr == text::_value_FULL
                        || decodedPtr == text::_source_FULL
                        || decodedPtr == text::_regexpBody_FULL
                        || decodedPtr == text::_regexpFlags_FULL) {
                        //printf(", ignNextStrval if this is a key");
                        // if it's a key.
                        // we should ignore the next value.
                        // string values to ignore (arbitrary or 
                        // rare compression gain) conveniently 
                        // correspond 1:1 with these keys that 
                        // directly precede any of them. 
                        // (e.g. there is never a list with some 
                        // or all strings to ignore)
                        ignoreNextStrval = (decodeIdx >=0)?2:1;
                    }

                    strcpy(decoded + iOut, decodedPtr);
                    while (decoded[iOut] != 0) {
                        ++iOut;
                    }
                    if (decodeIdx == -1) {
                        decoded[iOut] = block[b];
                        ++iOut;
                    }
                    ++b;
                    continue;
                } else {
                    // no decoded string here... 
                    //  false positive from the marker.
                    //printf("false positive from marker sequence in json decompress. null ptr at %i, %i \n", firstDecodeIdx, getDecodeIdx(block[b]));
                    decoded[iOut] = lastChar;
                    ++iOut;
                }   
            } else if (sequenceStep > NONE) {
                sequenceStep = NONE;
                continue; 
                //check for the init of a new sequence 
                // at this character.
            }

            decoded[iOut] = block[b];
            ++iOut;
            
            ++b;
        }
        if (b == curBlockSize) {
            free (blocks[curBlock]);
            curBlock++;

            if (curBlock == lastBlock){
                curBlockSize = lastBlockSize;
            }
            if (curBlock <= lastBlock) {
                block = blocks[curBlock];
                //printf("%s", string(block, curBlockSize-1).data());
            }
            b=0;
        }

    }
    //    decoded[iOut] = 0;
    decoded = (char *) realloc(decoded, iOut+1);
    lenOut = iOut;
    //printf(" final decoded: \n%s\n", decoded);
    //printf("eventual input length was %li \n", curBlockSize + (((long) lastBlock)*blockSize));
    //    return decoded;
}
#endif
