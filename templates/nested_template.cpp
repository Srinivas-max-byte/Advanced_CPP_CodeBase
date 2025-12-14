#include <iostream>
#include <vector>
#include <string>
#include <list>

namespace CustomSort
{
    template <typename T>
    class ascending
    {
    public:
        bool compare(T val1, T val2)
        {
            return (val1 < val2 ? true : false);
        }
    };

    template <typename T>
    class descending
    {
    public:
        bool compare(T val1, T val2)
        {
            return (val1 > val2 ? true : false);
        }
    };

    /*
        This is a template declaration with two template parameters. Let me break it down:

        First parameter: class Iter

            Declares a template type parameter named Iter
            It will be deduced as the iterator type when the function is called
            Example: when you call sorting(vect.begin(), ...), Iter is deduced as std::vector<int>::iterator
        
        Second parameter: template <class> class Comparator

            This is a template-template parameter — a template that takes another template as an argument
            template <class> describes the "shape" of the comparator template (it must be a template taking one type parameter, it can be any number depending on the need)
            class Comparator is the name of this template-template parameter
            Example: when you call sorting(..., CustomSort::ascending<int>()), Comparator is deduced as the ascending template (not ascending<int>, just the template itself)
            Analogy

        class Iter — like a variable that holds a type
        template <class> class Comparator — like a variable that holds a template

        Real-world mapping in your code: CustomSort::sorting(vect.begin(), vect.end(), CustomSort::ascending<int>());
            Iter ← deduced as std::vector<int>::iterator
            Comparator ← deduced as ascending (the template)
            Inside the function: Comparator<typename std::iterator_traits<Iter>::value_type> becomes ascending<int> (instantiating the template with int)
    */

    template <class Iter, template <class> class Comparator>
    void sorting(Iter begin, Iter end, Comparator<typename std::iterator_traits<Iter>::value_type> comp)
    {
        for (Iter i = begin; i != end; ++i) // begin: an object that behaves like a pointer — you can dereference it (*i), increment it (++i), compare it (i != end), etc.
        {
            std::cout << (int *)i << " ";
            for (Iter j = i; j != end; ++j)
            {
                if (comp.compare(*j, *i)) // if *j should come before *i
                    std::swap(*i, *j);    // std::swap() exchanges the values of two objects, not their positions/addresses using a temporary variable.
            }
        }
    }
};

int main()
{
    std::vector<int> vect = {12, 22, 3, 4, 56, 7, 8};
    CustomSort::sorting(vect.begin(), vect.end(), CustomSort::ascending<int>());

    for (auto itr : vect)
    {
        std::cout << itr << " ";
    }

    std::cout << std::endl;
    CustomSort::sorting(vect.begin(), vect.end(), CustomSort::descending<int>());

    for (auto itr : vect)
    {
        std::cout << itr << " ";
    }
    return 0;
}