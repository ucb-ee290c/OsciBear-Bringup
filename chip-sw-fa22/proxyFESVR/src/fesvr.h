#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "tsi.h"

/* Memory pool virtualization is currently not supported, meaning only host can initiate requests via channel A.
 * Support will be added later by adding a response function that polls and respond. 
 */
class Fesvr {
    public:
        virtual int init(int comport);
        int read(size_t addr, size_t *content, size_t size);
        int write(size_t addr, size_t content, size_t size);
        int loadElf(char* file, size_t addr);
        int run(size_t addr);
        //int memoryPoll();
};

class FesvrFpgaUart {
    public:
        int init(int comport);
        
    private:
        TsiFpgaUart comport;
};