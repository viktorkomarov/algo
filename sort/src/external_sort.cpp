#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <string>
#include <limits>
#include <sstream>
#include <cstdio>
#include <memory>
#include "sort.hpp"

constexpr int BUFF_SIZE = 4096;

class FileBuff{
public:
    FileBuff(const std::string& path, size_t buff_size) : size(buff_size), buff(buff_size), path(path), file(std::make_unique<std::ifstream>(new std::ifstream(path)))
    {
        fill_buff();
    }

    void close()
    {
        file->close();
        remove(path.c_str());
    }

    bool empty() { return file->eof() && (cur == size);}

    int front()
    {
        if (cur == size) fill_buff();

        return buff[cur];
    }

    void pop(){++cur;}
private:
    void fill_buff()
    {
        cur = size;
        while (file && cur)
        {
            file->operator>>(buff[size - cur]);
            --cur;
        }
        cur = 0;
    }

    size_t size;
    size_t cur = 0;
    std::vector<int> buff;
    const std::string& path;
    std::unique_ptr<std::ifstream> file;
};

template<typename Buff, typename Target>
void merge(std::vector<Buff>& arrs, Target&& target)
{
    while (std::count_if(arrs.begin(), arrs.end(), [](Buff q){return !q.empty();}) != 1)
    {
        auto min = INT64_MAX;
        size_t min_id = 0;
        std::vector<int> temp(arrs.size());
        for(size_t i = 0; i < arrs.size(); ++i)
        {
            if(!arrs[i].empty() && arrs[i].front() < min){
                min_id = i;
                min = arrs[i].front();
            }
        }
        target.push_back(min);
        arrs[min_id].pop();
    }
    
    for(size_t i = 0; i < arrs.size(); ++i)
    {
        while (!arrs[i].empty())
        {
            target.push_back(arrs[i].front());
            arrs[i].pop();
        }
    }
}

std::queue<int> to_queue(std::vector<int>& num)
{
    std::queue<int> q;
    for (auto it = num.begin(); it != num.end(); ++it)
    {
        q.push(*it);
    }
    return q;
}

std::vector<int> sort(std::vector<int>& arr)
{
   if (arr.size() <= 1)
      return arr;
 
   std::vector<int> left, right,result;
   size_t middle = (arr.size()+ 1) / 2;
 
   for (size_t i = 0; i < middle; i++) {
      left.push_back(arr[i]);
   }

   for (size_t i = middle; i < arr.size(); i++) {
      right.push_back(arr[i]);
   }
 
   left = sort(left);
   right = sort(right);

   std::vector<std::queue<int>> merged{to_queue(left), to_queue(right)};
   merge(merged, result); 
   return result;
}

std::string sort_and_save(std::vector<int>& buf, int ntn)
{
    buf = sort(buf);
    std::stringstream ss;
    ss << ntn << "_file";

    std::fstream file(ss.str(), std::fstream::in | std::fstream::trunc); // TODO::use binary mode
    for(size_t i = 0; i < buf.size(); ++i)
    {
        if (i != 0) file << " ";
        file << buf[i];
    }
    file.close();

    return ss.str();
}

void external_sort(std::string path)
{
    BinaryFile src(std::move(path));

    std::vector<std::string> sorted_buckets;
    while (bin.is_end())
    {
        auto bucket = bin.read_bucket(BUFF_SIZE);
        sorted_buckets.push_back(sort_and_save(std::move(bucket), sorted_buckets.size()));
    }

    std::vector<FileBuff> files;
    for(auto it = sorted_buckets.begin(); it != sorted_buckets.end(); ++it)
    {
        files.emplace_back(*it, BUFF_SIZE/sorted_buckets.size());
    }

    merge(files, bin);
}

int main()
{
    std::fstream src("test", std::ios::binary | std::ios::in | std::ios::out);
    BinaryFile bin("result");

    external_sort(src, bin);
}