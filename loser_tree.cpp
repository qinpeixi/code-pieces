#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cassert>

class Foo {
public:
    Foo() {}
    explicit Foo(int v): _v(v) {}
    Foo(const Foo& foo) { _v = foo._v; }
    int value() const { return _v; }
    Foo& operator=(const Foo& foo) { _v = foo._v; return *this; }
    bool operator==(const Foo& foo) { return _v == foo._v; }

private:
    int _v;
};
std::ostream& operator<<(std::ostream& os, Foo& foo) {
    return os << foo.value();
}

extern const Foo FOO_MAX(INT_MAX);

namespace  std {
template<>
class less<Foo> : std::binary_function<Foo, Foo, bool>
{
public:
    bool operator() (const Foo& x, const Foo& y) const {
        return x.value() < y.value();
    }
};
} // namespace std

template<class ValueType>
class FooContainer
{
public:
    ValueType operator[](size_t idx) const { return _container[idx]; }
    size_t size() const { return _container.size(); }
    void push_back(const ValueType& value) {
        _container.push_back(value);
    }

private:
    std::vector<ValueType> _container;
};

template< class ValueType,
          class ContainerType,
          const ValueType& max_value,
          class Compare = std::less<ValueType> >
class LoserTree
{
public:
    LoserTree(const ContainerType* ways, size_t num) :
        _num(num), _ways(ways), _indexes(new size_t[_num]),
        _data(new ValueType[_num]), _losers(new int[_num])
    {
        std::fill(_indexes, _indexes + _num, 0);
        std::fill(_losers, _losers + _num, -1);
        for (int way_idx = _num-1; way_idx >= 0; --way_idx) {
            if (_indexes[way_idx] == _ways[way_idx].size()) {
                _data[way_idx] = max_value;
            } else {
                _data[way_idx] = _ways[way_idx][_indexes[way_idx]];
            }
            adjust(way_idx);
        }
    }

    ~LoserTree() {
        delete[] _indexes;
        delete[] _losers;
        delete[] _data;
    }

    bool extract_one(ValueType& v) {
        int way_idx = _losers[0];
        if (_data[way_idx] == max_value)
            return false;
        v = _data[way_idx];
        if (++_indexes[way_idx] == _ways[way_idx].size()) {
            _data[way_idx] = max_value;
        } else {
            _data[way_idx] = _ways[way_idx][_indexes[way_idx]];
        }
        adjust(way_idx);
        return true;
    }

private:
    size_t _num;
    const ContainerType* _ways;
    size_t* _indexes;
    ValueType* _data;
    int* _losers;

    void adjust(int winner_idx) {
        using std::swap;
        // _losers[loser_idx_idx] is the index of the loser in _data
        int loser_idx_idx = (winner_idx + _num) / 2;
        while (loser_idx_idx != 0 && winner_idx != -1) {
            if (_losers[loser_idx_idx] == -1 ||
                    !Compare()(_data[winner_idx],  _data[_losers[loser_idx_idx]]))
                swap(winner_idx,_losers[loser_idx_idx]);
            loser_idx_idx /= 2;
        }
        _losers[0] = winner_idx;
    }
};

/*
 * input format:
 * 1 10 100 1000
 * 2 20 200 2000
 * 3 30 300
 * 4 40 400 4000 40000
 */
std::vector<std::vector<int> > get_input()
{
    std::vector<std::vector<int> > data;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::vector<int> tmp_data;
        std::istringstream iss(line);
        std::copy(std::istream_iterator<int>(iss), std::istream_iterator<int>(), std::back_inserter(tmp_data));
        data.push_back(tmp_data);
    }

    for (size_t i = 0; i < data.size(); ++i) {
        std::copy(data[i].begin(), data[i].end(), std::ostream_iterator<int>(std::cout, ", "));
        std::cout << std::endl;
    }

    return data;
}

template<class ValueType, class ContainerType>
std::vector<ContainerType> generate_data()
{
    const int way_num = 20;
    std::vector<ContainerType> data(way_num);
    for (int num = 0; num < 10/*100000*/; ++num) {
        data[rand() % way_num].push_back(ValueType(num));
    }

    return data;
}

void test_foo()
{
    std::vector<FooContainer<Foo> > data = generate_data<Foo, FooContainer<Foo> >();
    /*
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            //std::cout << data[i][j] << ", ";
            Foo foo = data[i][j];
            std::cout << foo << ", ";
        }
        std::cout << std::endl;
    }
    */

    LoserTree<Foo, FooContainer<Foo>, FOO_MAX> lt(data.data(), data.size());
    Foo v;
    Foo correct_res(0);
    while(lt.extract_one(v)) {
        //assert(v == correct_res);
        //correct_res = Foo(correct_res.value()+1);
        std::cout << v.value() << ", ";
    }
    std::cout << std::endl;
}

extern const int int_max = INT_MAX;
void test()
{
    std::vector<std::vector<int> > data = generate_data<int ,std::vector<int> >();
    LoserTree<int, std::vector<int>, int_max> lt(data.data(), data.size());
    int v;
    int correct_res(0);
    while (lt.extract_one(v)) {
        assert(v == correct_res++);
        std::cout << v << ", ";
    }
    std::cout << std::endl;
}

int main()
{
    test_foo();
    test();

    return 0;
}

