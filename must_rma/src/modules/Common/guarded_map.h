#include <map>
#include <mutex>
template <class K, class V, class Compare = std::less<K>, class Allocator = std::allocator<std::pair<const K, V> > >
class guarded_map {
    private:
        std::map<K, V, Compare, Allocator> _map;
        std::mutex _m;
        typedef typename std::map<K, V, Compare, Allocator>::iterator iterator;
        typedef typename std::map<K, V, Compare, Allocator>::const_iterator const_iterator;

    public:
        void set(K key, V value) {
            std::lock_guard<std::mutex> lk(this->_m);
            this->_map[key] = value;
        }

        iterator find (const K& key) {
            std::lock_guard<std::mutex> lk(this->_m);
            return this->_map.find(key);
        }
        
        const_iterator find (const K& key) const {
            std::lock_guard<std::mutex> lk(this->_m);
            return this->_map.find(key);
        }
        
        iterator end () {
            std::lock_guard<std::mutex> lk(this->_m);
            return this->_map.end();
        }
        
        const_iterator end (const K& key) const {
            std::lock_guard<std::mutex> lk(this->_m);
            return this->_map.end();
        }
        
        V& operator[] (const K& key){
            std::lock_guard<std::mutex> lk(this->_m);
            return this->_map[key];
        }
//        V& operator[] (K&& k);

        V & get(K key) {
            std::lock_guard<std::mutex> lk(this->_m);
            return this->_map[key];
        }

        bool empty() {
            std::lock_guard<std::mutex> lk(this->_m);
            return this->_map.empty();
        }

        // other public methods you need to implement
};
