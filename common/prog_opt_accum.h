// -*- C++ -*-

#if !defined(TOWERSIM_COMMON_PROG_OPT_ACCUM_H)
#define TOWERSIM_COMMON_PROG_OPT_ACCUM_H

#include <boost/program_options.hpp>

namespace prog_opt_ext
{
   namespace prog_opt = boost::program_options;

   template <class T, class charT = char>
   class accumulating_value : public prog_opt::typed_value<T,charT>
   {
      public:
         accumulating_value(T* store_to=0)
         : prog_opt::typed_value<T,charT>(store_to), origin(0)
         {
            prog_opt::typed_value<T,charT>::zero_tokens();
         }

         accumulating_value* default_value(const T &v)
         {
            // setting a default value sets the origin to that value
            origin = v;
            prog_opt::typed_value<T,charT>::default_value(v);
            return this;
         }

         accumulating_value* default_value(const T &v,const std::string& textual)
         {
            // setting a default value sets the origin to that value
            origin = v;
            prog_opt::typed_value<T,charT>::default_value(v, textual);
            return this;
         }

         void xparse(boost::any& value_store,
                     const std::vector<std::basic_string<charT> >& new_tokens) const
         {
            // if this is the first occurrence of the option, initialize it
            // to the origin.
            if (value_store.empty())
               value_store = boost::any(origin);
            ++boost::any_cast<T&>(value_store);
         }

      private:
         T origin; // the numeric origin from which to increment upward.
   };

   template <class T>
   accumulating_value<T>*
   accum_value(T *v)
   {
      accumulating_value<T>* r =  new accumulating_value<T>(v);
      return r;
   }

   template <class T>
   accumulating_value<T>*
   accum_value()
   {
      return accum_value<T>(0);
   }

} // namespace prog_opt_ext

#endif
