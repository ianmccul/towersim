#include "method.h"

int main()
{
#if 0
   method Meth("Cambridge");

   Meth.add_block(block(interpret_pn(6, "&x36x14x12x36x14x56"), true));

   block CallBlock(interpret_pn(6, "12"));
   CallBlock.add_call(call::bob(), interpret_pn(6, "14"));
   CallBlock.add_call(call::single(), interpret_pn(6, "1234"));

   Meth.add_block(CallBlock);
#endif

   //   method Meth = ParseMethod(6, "&x36x14x12x36x14x56,|(12)(14)(1234)");
   //   method Meth = ParseMethod(5, "|5.1.3.(1)()(145).*3.1|5.3.1.(3)()(345).1.3");
   //   method Meth = ParseMethod(7, "|(7)(5)(567).1.3.1.*3.1|(7)(5)(567).3.1.3.1.3");
   
   method Meth = ParseMethod(7, "3.1.7.1.5.(1.7.1)(\"r,Red\"3.7.3).7.5.1.7.1.7.(1.7.1)(\"b,Blue\"3.7.3)(\"d,Double\"3.34567.3).7.1.5.1.5.(1.7.1)(\"g,Green\"3.7.3).7.1.7.1.7");

   method_cursor c(Meth);

   while (!(++c).is_rounds())
   {
      std::cout << (*c) << ' ';
      if (c.is_call_available_now())
      {
	 std::set<call> Calls = c.calls_available_now();
	 for (std::set<call>::const_iterator I = Calls.begin(); I != Calls.end(); ++I)
	 {
	    std::cout << I->Abbrev << ' ';
	 }
      }
      if (c.is_splice_point())
	 std::cout << "splice ";

      std::cout << '\n';

      if (c.is_division())
	 std::cout << "------\n";
   }
   std::cout << (*c) << '\n';
 }
