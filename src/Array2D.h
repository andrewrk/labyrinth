
#ifndef _ARRAY_2D_
#define _ARRAY_2D_

template <class T>
class Array2D {
    public:
        Array2D(int width, int height);
        ~Array2D();

        // get an element at x,y
        T& get(int x, int y) const; 

        // set an element at x,y
        void set(int x, int y, T item);

        // resize the array
        void redim(int new_width, int new_height);

    private:
        int m_width, m_height;
        T * m_array;

};

template <class T>
Array2D<T>::Array2D(int width, int height) :
    m_width(width),
    m_height(height)
{
    m_array = new T[width*height];
}

template <class T>
Array2D<T>::~Array2D() {
    delete[] m_array;
}

template <class T>
T& Array2D<T>::get(int x, int y) const {
    return m_array[y*m_height+x];
}

template <class T>
void Array2D<T>::set(int x, int y, T item) {
    m_array[y*m_height+x] = item;
}

template <class T>
void Array2D<T>::redim(int new_width, int new_height){
    // skip if we're already here
    if( new_width == m_width && new_height == m_height ) return;

    delete[] m_array;
    m_array = new T[new_width*new_height];
}

#endif
