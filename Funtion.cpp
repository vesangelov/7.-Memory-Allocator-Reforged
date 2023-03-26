#include "Defines.h"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

struct Block{
    bool operator==(const Block& other) const {
        if(other.begin_ >= this->begin_ && other.begin_ < this->end_){
            return true;
        }

        if(other.end_ > this->begin_ && other.end_ <= this->end_){
            return true;
        }

        return false;
    }

    bool operator<(const Block& other) const {
        return this->end_ <= other.begin_;
    }
    int begin_;
    int end_;
    bool is_multiple_;
};

class Allocator{
public:
    ErrorCode allocate(int index, int size){
        Block block{index, index + 1,
                    size == ALLOCATION_MULTIPLE_SIZE};
        auto i = allocations.insert(block);

        return i.second ? EXECUTE_SUCCESS : MEMORY_LEAK;
    }

    ErrorCode deallocate(int index, int size){

        Block block{index, index + size,
                    size == ALLOCATION_MULTIPLE_SIZE};
        auto i = allocations.find(block);

        if(i == allocations.end()){
            return DOUBLE_FREE;
        }

        if(index != i->begin_){
            return DOUBLE_FREE;
        }

        //if(size != i->end_ - i->begin_){
        if(i->is_multiple_ && size == 1){
            return ALLOCATE_DEALLOCATE_MISMATCH;
        }

        if(!i->is_multiple_ && size != 1){
            return ALLOCATE_DEALLOCATE_MISMATCH;
        }

        allocations.erase(i);
        return EXECUTE_SUCCESS;
    }

private:
    std::set<Block> allocations;
};

static Allocator allocator;

ErrorCode executeCommand(const std::string &       command,
                         std::vector<MemoryNode> & memory){

    int index = 0;
    if(command[0] == 'A'){
        if(command[9] == 'S'){
            index = std::stoi(command.substr(15));
            return allocator.allocate(index, 1);
        }
        else {
            index = std::stoi(command.substr(18));
            return allocator.allocate(index, ALLOCATION_MULTIPLE_SIZE);
        }
    }
    else {
        if(command[11] == 'S'){
            index = std::stoi(command.substr(17));
            return allocator.deallocate(index, 1);
        }
        else {
            index = std::stoi(command.substr(20));
            return allocator.deallocate(index, ALLOCATION_MULTIPLE_SIZE);
        }
    }
}

void printResult(const ErrorCode     errorCode,
                 const std::string & command){

    std::cout << command << " - ";
    switch (errorCode) {
        case EXECUTE_SUCCESS:
            std::cout << "success\n";
            break;
        case MEMORY_LEAK:
            std::cout << "memory leak prevented, will not make allocation\n";
            break;
        case DOUBLE_FREE:
            std::cout << "system crash prevented, will skip this deallocation\n";
            break;
        case ALLOCATE_DEALLOCATE_MISMATCH:
            std::cout << "Warning allocate/deallocate mismatch, will skip this deallocation\n";
            break;
    }
}
