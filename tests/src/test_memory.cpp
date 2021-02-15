
#include <iostream>
#include <memory>
#include <mutex>

/**
 * + debugging -> print stats
 * + multithreading support
 * sort/merge pages
 * profile with normal malloc
 * unit tests
 */

class MemoryManager
{
private:
    struct MemoryPage
    {
        struct MemoryChunk
        {
            std::size_t     size = 0;
            char*           addr = nullptr;

            // double linked list
            MemoryChunk*    prevChunk = nullptr;
            MemoryChunk*    nextChunk = nullptr;
        };

        char*           pageAddr = nullptr;
        std::size_t     pageSize = 0;
        MemoryPage*     nextPage = nullptr;
        MemoryChunk*    _usedChunks = nullptr;
        MemoryChunk*    _freeChunks = nullptr;

        MemoryPage(std::size_t requestedSize)
        {
            pageAddr = reinterpret_cast<char*>(this + sizeof(*this));
            pageSize = requestedSize - sizeof(*this);

            // first memory chunk
            _freeChunks = reinterpret_cast<MemoryChunk*>(pageAddr);
            _freeChunks->size = requestedSize - sizeof(MemoryChunk);
            _freeChunks->addr = pageAddr + sizeof(MemoryChunk);
            _freeChunks->prevChunk = nullptr;
            _freeChunks->nextChunk = nullptr;
        }

        void* tryAlloc(std::size_t requestedSize)
        {
            // must keep data before the actually returned pointer

            // get big enough chunk
            // try to subdivide chunk if possible
            // return select chunk addr

            // std::cout << "tryAlloc (requestedSize=" << requestedSize << ")" << std::endl;

            for (MemoryChunk* currFreeChunk = _freeChunks; currFreeChunk; currFreeChunk = currFreeChunk->nextChunk)
            {
                // std::cout << "looping free chunks (" << currFreeChunk->size << ")" << std::endl;

                // chunk big enough?
                if (currFreeChunk->size < requestedSize)
                    continue;

                // std::cout << "chunk big enough" << std::endl;

                // subdivide chunk?
                std::size_t leftOverSize = currFreeChunk->size - requestedSize;
                if (leftOverSize > sizeof(MemoryChunk) + 128) // minimum x bytes big
                {
                    // std::cout << "subdivide chunk" << std::endl;
                    // std::cout << "currFreeChunk=" << currFreeChunk << " (" << currFreeChunk->size << ")" << std::endl;

                    //
                    // build and link next chunk

                    MemoryChunk* nextFreeChunk = reinterpret_cast<MemoryChunk*>(currFreeChunk->addr + requestedSize);
                    nextFreeChunk->size = leftOverSize - sizeof(MemoryChunk);
                    nextFreeChunk->addr = currFreeChunk->addr + requestedSize + sizeof(MemoryChunk);
                    nextFreeChunk->prevChunk = nullptr;
                    nextFreeChunk->nextChunk = _freeChunks;
                    if (_freeChunks)
                        _freeChunks->prevChunk = nextFreeChunk;
                    _freeChunks = nextFreeChunk;

                    // std::cout << "nextFreeChunk=" << nextFreeChunk << " (" << nextFreeChunk->size << ")" << std::endl;

                    // reduce current chunk

                    currFreeChunk->size = requestedSize;
                }

                // move free chunk to used chunk

                //
                // remove chunk

                if (currFreeChunk->prevChunk)
                    currFreeChunk->prevChunk->nextChunk = currFreeChunk->nextChunk;
                if (currFreeChunk->nextChunk)
                    currFreeChunk->nextChunk->prevChunk = currFreeChunk->prevChunk;

                // special case
                if (_freeChunks == currFreeChunk)
                    _freeChunks = currFreeChunk->nextChunk;

                //
                // add chunk

                currFreeChunk->prevChunk = nullptr;
                currFreeChunk->nextChunk = _usedChunks;
                if (_usedChunks)
                    _usedChunks->prevChunk = currFreeChunk;
                _usedChunks = currFreeChunk;

                // std::cout << "currFreeChunk=" << currFreeChunk << " (" << currFreeChunk->size << ")" << std::endl;
                // std::cout << "currFreeChunk->addr=" << reinterpret_cast<void*>(currFreeChunk->addr) << std::endl;

                print();

                sortChunks();
                sortChunks();
                sortChunks();

                return _usedChunks->addr;
            }

            return nullptr;
        }

        bool tryDealloc(void* addrToDel)
        {
            // check if in the adress range of the page
            // get memory chunk to deallocate
            // merge with previous/next chunk

            // check if in the adress range of the page
            if (addrToDel < pageAddr || addrToDel > pageAddr + pageSize)
                return false;

            // std::cout << "tryDealloc (addrToDel=" << addrToDel << ")" << std::endl;

            // int iterationLeft = 20;

            // get memory chunk to deallocate
            MemoryChunk* currUsedChunk = _usedChunks;
            while (currUsedChunk)
            {
                // if (--iterationLeft <= 0)
                // {
                //     std::cout << "EXIT" << std::endl;
                //     // throw std::runtime_error("nor more iteration left");
                //     exit(1);
                // }

                // std::cout << "looping used chunks (" << currUsedChunk << ", " << currUsedChunk->size << ")" << std::endl;
                // std::cout << "addr=" << static_cast<void*>(currUsedChunk->addr) << std::endl;

                if (currUsedChunk->addr == addrToDel)
                {
                    // std::cout << "addr match" << std::endl;

                    //
                    // remove chunk

                    if (currUsedChunk->prevChunk)
                        currUsedChunk->prevChunk->nextChunk = currUsedChunk->nextChunk;
                    if (currUsedChunk->nextChunk)
                        currUsedChunk->nextChunk->prevChunk = currUsedChunk->prevChunk;

                    // special case
                    if (_usedChunks == currUsedChunk)
                        _usedChunks = currUsedChunk->nextChunk;

                    //
                    // add chunk

                    if (_freeChunks)
                        _freeChunks->prevChunk = currUsedChunk;

                    currUsedChunk->nextChunk = _freeChunks;
                    _freeChunks = currUsedChunk;

                    // sortChunks();

                    return true;
                }

                currUsedChunk = currUsedChunk->nextChunk;
            }

            return false;
        }

        void print()
        {
            std::cout << "===== PRINT =====" << std::endl;
            std::cout << "pageAddr=" << static_cast<void*>(pageAddr) << std::endl;
            std::cout << "pageSize=" << pageSize << std::endl;
            std::cout << "nextPage=" << static_cast<void*>(nextPage) << std::endl;

            std::cout << "USED:" << std::endl;
            MemoryChunk* currUsedChunk = _usedChunks;
            while (currUsedChunk)
            {
                std::cout << "=> used=" << static_cast<void*>(currUsedChunk->addr) << " [" << reinterpret_cast<long long>(currUsedChunk->addr) << "] (" << currUsedChunk->size << ")" << std::endl;
                currUsedChunk = currUsedChunk->nextChunk;
            }

            std::cout << "FREE:" << std::endl;
            MemoryChunk* currFreeChunk = _freeChunks;
            while (currFreeChunk)
            {
                std::cout << "=> free=" << static_cast<void*>(currFreeChunk->addr) << " [" << reinterpret_cast<long long>(currFreeChunk->addr) << "] (" << currFreeChunk->size << ")" << std::endl;
                currFreeChunk = currFreeChunk->nextChunk;
            }

            std::cout << "===== /PRINT =====" << std::endl;
        }

        void sortChunks()
        {
            // return;

            std::cout << "SORT()" << std::endl;

            // sort used

            do
            {
                bool mustLoopAgain = false;
                MemoryChunk* currUsedChunk = _usedChunks;
                while (currUsedChunk)
                {
                    if (currUsedChunk->nextChunk &&
                        reinterpret_cast<long long>(currUsedChunk->nextChunk->addr) < reinterpret_cast<long long>(currUsedChunk->addr))
                    {
                        // swap: curr <-> next

                        /**
                         *          /<------\
                         *    /----/--->\    \
                         * O <--> C <--> N <--> O
                         *  \<-----/    \----->/
                         */

                        MemoryChunk* currNextChunk = currUsedChunk->nextChunk;

                        auto local_printLinks = [&currUsedChunk, &currNextChunk]() {

                            std::cout << std::endl;
                            std::cout << std::endl;
                            std::cout << "step" << std::endl;
                            std::cout
                                << "CURR: "
                                << static_cast<void*>(currUsedChunk->prevChunk ? currUsedChunk->prevChunk->addr : nullptr)
                                << " <--> "
                                << static_cast<void*>(currUsedChunk->addr)
                                << " <--> "
                                << static_cast<void*>(currUsedChunk->nextChunk ? currUsedChunk->nextChunk->addr : nullptr)
                                << std::endl;
                            std::cout
                                << "NEXT: "
                                << static_cast<void*>(currNextChunk->prevChunk ? currNextChunk->prevChunk->addr : nullptr)
                                << " <--> "
                                << static_cast<void*>(currNextChunk->addr)
                                << " <--> "
                                << static_cast<void*>(currNextChunk->nextChunk ? currNextChunk->nextChunk->addr : nullptr)
                                << std::endl;
                            std::cout << "step" << std::endl;
                            std::cout << std::endl;
                            std::cout << std::endl;
                        };

                        local_printLinks();

                        currUsedChunk->nextChunk = currNextChunk->nextChunk;
                        currNextChunk->nextChunk = currUsedChunk;

                        currNextChunk->prevChunk = currUsedChunk->prevChunk;
                        currUsedChunk->prevChunk = currNextChunk;

                        if (currUsedChunk->prevChunk)
                            currUsedChunk->prevChunk->nextChunk = currUsedChunk;
                        if (currNextChunk->nextChunk)
                            currNextChunk->nextChunk->prevChunk = currNextChunk;

                        // special case
                        if (_usedChunks == currUsedChunk)
                            _usedChunks = currNextChunk;

                        local_printLinks();

                        mustLoopAgain = true;
                        break;
                    }

                    currUsedChunk = currUsedChunk->nextChunk;
                }

                if (!mustLoopAgain)
                    break;
            }
            while (true);

            // return;

            // // sort free

            // do
            // {
            //     bool mustLoopAgain = false;
            //     MemoryChunk* currFreeChunk = _freeChunks;
            //     while (currFreeChunk)
            //     {
            //         if (currFreeChunk->nextChunk &&
            //             currFreeChunk->nextChunk->addr < currFreeChunk->addr)
            //         {
            //             // swap

            //             // O <-> O <-> O <-> O

            //             MemoryChunk* nextFreeChunk = currFreeChunk->nextChunk;

            //             currFreeChunk->nextChunk = nextFreeChunk->nextChunk;
            //             nextFreeChunk->nextChunk = currFreeChunk;

            //             nextFreeChunk->prevChunk = currFreeChunk->prevChunk;
            //             currFreeChunk->prevChunk = nextFreeChunk;

            //             // special case
            //             if (currFreeChunk == _freeChunks)
            //                 _freeChunks = nextFreeChunk;

            //             mustLoopAgain = true;
            //             break;
            //         }

            //         currFreeChunk = currFreeChunk->nextChunk;
            //     }

            //     if (!mustLoopAgain)
            //         break;
            // }
            // while (true);
        }

        void mergeChunks()
        {
        }
    };

    std::mutex  _mutex;
    MemoryPage* _page = nullptr;

public: // ctor/dtor
    MemoryManager() = default;

    ~MemoryManager()
    {
        MemoryPage* currPage = _page;
        while (currPage)
        {
            MemoryPage* toDelete = currPage;
            currPage = currPage->nextPage;

            std::free(toDelete);
        }
    }

public:
    void* allocate(std::size_t requestedSize)
    {
        // check if page with enough size left available
        // allocate big enough page if none

        std::unique_lock<std::mutex> lock(_mutex);

        MemoryPage* currPage = _page;
        while (currPage)
        {
            void* addr = currPage->tryAlloc(requestedSize);
            if (addr)
                return addr;

            currPage = currPage->nextPage;
        }

        // must be big enough to contain a MemoryPage
        std::size_t actualSize = 1024; // start at 1Ko

        // double the size until big enough
        while (actualSize < requestedSize + sizeof(MemoryPage))
            actualSize *= 2;

        // allocate page
        char* pageAddr = reinterpret_cast<char*>(std::malloc(actualSize));

        MemoryPage* newPage = new (pageAddr) MemoryPage(actualSize);

        newPage->nextPage = _page;
        _page = newPage;

        return _page->tryAlloc(requestedSize);
    }

    void deallocate(void* address)
    {
        std::unique_lock<std::mutex> lock(_mutex);

        MemoryPage* currPage = _page;
        while (currPage)
        {
            if (currPage->tryDealloc(address))
                return;

            currPage = currPage->nextPage;
        }
    }

    void print()
    {
        std::cout << std::endl;
        std::cout << "----- PRINT -----" << std::endl;

        MemoryPage* currPage = _page;
        while (currPage)
        {
            currPage->print();
            currPage = currPage->nextPage;
        }

        std::cout << "----- /PRINT -----" << std::endl;
        std::cout << std::endl;
    }
};


static MemoryManager memManager;














// void* operator new (std::size_t size)
// {
//     std::cout << std::endl;
//     std::cout << "NEW" << std::endl;

//     memManager.print();

//     void* addr = memManager.allocate(size);

//     memManager.print();

//     std::cout << "/NEW" << std::endl;
//     std::cout << std::endl;

//     return addr;
// }

// void operator delete (void* address)
// {
//     std::cout << std::endl;
//     std::cout << "DELETE" << std::endl;

//     memManager.print();

//     memManager.deallocate(address);

//     memManager.print();

//     std::cout << "/DELETE" << std::endl;
//     std::cout << std::endl;
// }

// void operator delete (void* address, std::size_t size)
// {
//     static_cast<void>(size); // unused

//     std::cout << std::endl;
//     std::cout << "DELETE[]" << std::endl;

//     memManager.print();

//     memManager.deallocate(address);

//     memManager.print();

//     std::cout << "/DELETE[]" << std::endl;
//     std::cout << std::endl;
// }











// static std::size_t totalMemoryUsed = 0;

// void* operator new (std::size_t size)
// {
//     std::size_t oldMemoryUsed = totalMemoryUsed;

//     totalMemoryUsed += size;

//     std::cout << "memory +" << size << " (" << oldMemoryUsed << " -> " << totalMemoryUsed << ")" << std::endl;

//     return std::malloc(size);
// }

// void operator delete (void* address)
// {
//     std::cout << "memory2 (" << totalMemoryUsed << ")" << std::endl;

//     std::free(address);
// }

// void operator delete (void* address, std::size_t size)
// {
//     std::size_t oldMemoryUsed = totalMemoryUsed;

//     totalMemoryUsed -= size;

//     std::cout << "memory -" << size << " (" << oldMemoryUsed << " -> " << totalMemoryUsed << ")" << std::endl;

//     std::free(address);
// }


void test_memory()
{
    // std::cout << "totalMemoryUsed: " << totalMemoryUsed << std::endl;

    // memManager.print();

    {
        std::cout << std::endl;
        std::cout << "################" << std::endl;
        std::cout << "# auto pointer #" << std::endl;
        std::cout << "################" << std::endl;
        std::cout << std::endl;
        {
            auto pointerA = std::make_unique<char[]>(128);
            auto pointerB = std::make_unique<char[]>(128);

            // memManager.print();
        }
        std::cout << std::endl;
        std::cout << "################" << std::endl;
        std::cout << "# auto pointer #" << std::endl;
        std::cout << "################" << std::endl;
        std::cout << std::endl;
    }

    // memManager.print();

    {
        std::cout << std::endl;
        std::cout << "##################" << std::endl;
        std::cout << "# manual pointer #" << std::endl;
        std::cout << "##################" << std::endl;
        std::cout << std::endl;
        {
            char* pointer = new char[666];

            // memManager.print();

            delete[] pointer;
        }
        std::cout << std::endl;
        std::cout << "##################" << std::endl;
        std::cout << "# manual pointer #" << std::endl;
        std::cout << "##################" << std::endl;
        std::cout << std::endl;
    }

    // memManager.print();

    // std::cout << "totalMemoryUsed: " << totalMemoryUsed << std::endl;
}
