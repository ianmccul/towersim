// -*- C++ -*- $Id: display_attribute.h 974 2012-07-04 22:20:50Z uqimccul $

#if !defined(DISPLAY_ATTRIBUTE_H_HVJW4EUYHL)
#define DISPLAY_ATTRIBUTE_H_HVJW4EUYHL

#include "common/trace.h"

// utility function to display the attribute of a spirit parser
// from http://boost-spirit.com/home/2010/01/31/what-is-the-attribute-type-exposed-by-a-parser/

template <typename Expr, typename Iterator = boost::spirit::unused_type>
struct attribute_of_parser
{
    typedef typename boost::spirit::result_of::compile<
        boost::spirit::qi::domain, Expr
    >::type parser_expression_type;

    typedef typename boost::spirit::traits::attribute_of<
        parser_expression_type, boost::spirit::unused_type, Iterator
    >::type type;
};

template <typename T>
void display_attribute_of_parser(T const&)
{
    typedef typename attribute_of_parser<T>::type attribute_type;
    std::cout << tracer::typeid_name<attribute_type>() << std::endl;
}

#endif
