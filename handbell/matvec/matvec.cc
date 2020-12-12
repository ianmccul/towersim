
template <typename T>
matrix3<T>::matrix3(T init)
{
    for (int i = 0; i < 9; i++)
    {
        data[i] = init;
    }
}

template <typename T>
matrix3<T> matrix3<T>::identity()
{
    matrix3<T> Result(T());
    Result(0,0) = 1;
    Result(1,1) = 1;
    Result(2,2) = 1;
    return Result;
}

