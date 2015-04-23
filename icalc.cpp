//usage example
// ./a.out expression.txt
// expression.txt's content 5-(4+10)/2*5-1

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
template <typename Iterator>
class Grammar: public qi::grammar<Iterator, int(),ascii::space_type>
{
public:
	Grammar():Grammar::base_type(expr){
		using qi::int_;
		using qi::eps;
		using qi::_val;
		using namespace qi::labels;
		using qi::lexeme;
		expr = term[_val = _1]||+( ('+'>>term[_val+=_1])|('-'>>term[_val -= _1])) ;//左结合
		term = factor[_val=_1]||+( ('*'>>(factor)[_val*=_1])|('/'>>(factor)[_val /= _1])) ;//左结合
		factor = (int_|group)[_val = _1];
		group %= '('>>expr>>')';
	}
private:
	qi::rule<Iterator, int(),ascii::space_type> term;
	qi::rule<Iterator, int(),ascii::space_type> group;
	qi::rule<Iterator, int(),ascii::space_type> expr;
	qi::rule<Iterator, int(),ascii::space_type> factor;
};
int main(int argc, char** argv)
{
	char const* filename;
	if (argc > 1)
	{
		filename = argv[1];
	}
	else
	{
		//usage : ./a.out expression.txt
		std::cerr << "Error: No input file provided." << std::endl;
		return 1;
	}
	std::ifstream in(filename, std::ios_base::in);

	if (!in)
	{
		std::cerr << "Error: Could not open input file: "
			<< filename << std::endl;
		return 1;
	}
	std::string storage; // We will read the contents here.
	in.unsetf(std::ios::skipws); // No white space skipping!
	std::copy(
		std::istream_iterator<char>(in),
		std::istream_iterator<char>(),
		std::back_inserter(storage));
	Grammar<std::string::const_iterator> grammar;
	int ast;
	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = qi::phrase_parse(iter, end, grammar , ascii::space, ast);

	if (r && iter == end)
	{
		std::cout << "-------------------------\n";
		std::cout << "parsing succeeded\n";
		std::cout << "-------------------------\n";
		std::cout << "result: " << ast<<"\n";
		return 0;
	}
	else
	{
		std::string context(iter, (end-iter)>30?iter+30:end);
		std::cout << "-------------------------\n";
		std::cout << "parsing failed\n";
		std::cout << "stopped at: \": " << context << "...\"\n";
		std::cout << "-------------------------\n";
		return 1;
	}
}