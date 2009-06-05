//pelican aka sam dennis wrote this

#include <vector>
#include <memory>

template <class T, class A = std::allocator<T> >
class svector : public std::vector<T, A> {
		using std::vector<T, A>::begin;
        public:
                void erase(typename std::vector<T, A>::size_type i) {
                        std::vector<T, A> &vec = *this;
                        vec.erase(begin() + i);
                }
                void insert(typename std::vector<T, A>::size_type i, const T &v) {

                        std::vector<T, A> &vec = *this;
                        vec.insert(begin() + i, v);
                }
};