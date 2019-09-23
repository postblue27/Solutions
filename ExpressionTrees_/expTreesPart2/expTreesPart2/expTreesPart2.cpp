#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <stack>
#include <exception>
#include <sstream>
#include <cstdlib>
#include <vector>

//expression will store in list of char
std::string ReadExpression(std::ifstream& ifs) {
	std::string exp;
	getline(ifs, exp);
	return exp;
}

//function to check if a given char is operator
bool IsOperator(std::string op) {
	if (op == "+" || op == "-" || op == "*" || op == "/" || op == "<" || op == ">" || op == "==" || op == "!=" || op == "<=" || op == ">=" || op == "&&" || op == "||") {
		return true;
	}
	return false;
}

bool IsValid(std::string s) {
	//checking parentheses 
	//we will dynamically count parentheses by incrementing p_count when see '(' and decrementing it when see ')'
	//in the end of expression p_count should be 0. If not - return false
	//if p_count is less then 0 in any point of expression than expression is wrong and we return false
	int p_count = 0;
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '(') {
			p_count++;
		}
		if (s[i] == ')') {
			p_count--;
		}
		if (p_count < 0) {
			return false;
		}
		//after ')' can only stand another ')' or an operator
		if (i != s.length() - 1 && s[i] == ')' && (s[i + 1] != ')' && s[i + 1] != '+' && s[i + 1] != '-' && s[i + 1] != '*' && s[i + 1] != '/')) {
			return false;
		}
		//after '(' can only stand another '(' or a numeric symbol
		if (i != s.length() - 1 && s[i] == '(' && (s[i + 1] != '(' && !isdigit(s[i + 1]))) {
			return false;
		}
		//after operator can only stand '(' or a numeric symbol. Also operator can not be in the beginning or in the end of expression
		if (i != s.length() - 1 && IsOperator(std::string(1, s[i])) && (IsOperator(std::string(1, s[i + 1])) || (s[i + 1] != '(' && !isdigit(s[i + 1])))) {
			return false;
		}
		if ((i == 0 || i == s.length() - 1) && IsOperator(std::string(1, s[i]))) {
			return false;
		}
	}
	if (p_count == 0) {
		return true;
	}
	else {
		return false;
	}
}

struct Node;

int priority(char);
Node* NewNode(std::string);
void AppendNodes(std::stack<Node*>&, std::stack<Node*>&);

struct Node {
	std::string value;
	Node* left;
	Node* right;
	bool checked = false;
};

Node* NewNode(std::string v) {
	Node* newNode = new Node;
	newNode->value = v;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

void AppendNodes(std::stack<Node*>& nodes, std::stack<Node*>& operators) {
	Node* operatorNode = operators.top();
	operators.pop();
	operatorNode->left = nodes.top();
	nodes.pop();
	operatorNode->right = nodes.top();
	nodes.pop();
	nodes.push(operatorNode);
}

int priority(std::string op) {
	if (op == "||") {
		return 1;
	}
	if (op == "&&") {
		return 2;
	}
	if ((op == "==") || (op == "!=")) {
		return 3;
	}
	if ((op == "<") || (op == ">") || (op == ">=") || (op == "<=")) {
		return 4;
	}
	if ((op == "+") || (op == "-")) {
		return 5;
	}
	if ((op == "/") || (op == "*")) {
		return 6;
	}
}

std::stack<Node*> BuildTree(std::string inputString, std::stack<Node*> operands, std::stack<Node*> operators) {
	std::string operand = "";
	for (int i = 0; i < inputString.length(); i++) {
		//If it is Closing parenthesis, make into node, push it on stack.
		if (inputString[i] == '(') {
			operators.push(NewNode(std::string(1, inputString[i])));
		}

		//If it is an operator, then
		if (IsOperator(std::string(1, inputString[i])) || inputString[i] == '(' || inputString[i] == ')' || IsOperator(std::string(1,inputString[i]) + std::string(1, inputString[i+1]))) {
			//stop reading an operand and push it to nodes
			if (operand != "") {
				operands.push(NewNode(operand));
			}
			operand = "";

			std::string op = "";
			if (IsOperator(std::string(1, inputString[i]))) {
				op += std::string(1, inputString[i]);
			}

			if (IsOperator(std::string(1, inputString[i]) + std::string(1, inputString[i + 1]))) {
				op = "";
				op += std::string(1, inputString[i]);
				op += std::string(1, inputString[i+1]);
			}


			if (IsOperator(std::string(1, inputString[i])) || IsOperator(std::string(1, inputString[i]) + std::string(1, inputString[i + 1]))) {
				while (true) {
					//If stack is empty, make node and push operator on stack.
					if (operators.empty()) {
						operators.push(NewNode(op));
						break;
					}
					//If the top of stack is closing parenthesis, make node and push operator on stack.
					else if (operators.top()->value == "(") {
						operators.push(NewNode(op));
						break;
					}
					//If it has same or higher priority than the top of stack, make node and push operator on stack.
					else if ((priority(op) > priority(operators.top()->value)) || (priority(op) == priority(operators.top()->value))) {
						operators.push(NewNode(op));
						break;
					}
					//Else pop the operator from the stack, perform attachOperator and add it to treenodes. repeat step 5.
					else {
						AppendNodes(operands, operators);
					}
				}
				if (op.length() == 2) {
					i++;
				}
			}
		}
		else {
			operand += inputString[i];
			if (i == inputString.length() - 1) {
				operands.push(NewNode(operand));
			}
		}
		//If it is a opening parenthesis, pop operators from stack and perform attachOperator 
		//until a closing parenthesis is encountered. Pop and discard the closing parenthesis.
		if (inputString[i] == ')') {
			while (operators.top()->value != "(")
			{
				AppendNodes(operands, operators);
			}
			operators.pop(); // ')' is popped and discarded
		}
	}
	while (!operators.empty()) {
		AppendNodes(operands, operators);
	}
	return operands;
}

double EvaluateTree(Node* node) {
	double z;
	double x, y;
	if (IsOperator(node->value)) {
		x = EvaluateTree(node->right);
		y = EvaluateTree(node->left);
		if (node->value == "+")
			z = x + y;
		else if (node->value == "-")
			z = x - y;
		else if (node->value == "*")
			z = x * y;
		else if (node->value == "/")
			z = x / y;
		else if (node->value == ">")
			z = x > y;
		else if (node->value == "<")
			z = x < y;
		else if (node->value == ">=")
			z = x >= y;
		else if (node->value == "<=")
			z = x <= y;
		else if (node->value == "&&")
			z = x && y;
		else if (node->value == "||")
			z = x || y;
		else if (node->value == "!=")
			z = x != y;
		else if (node->value == "==")
			z = x == y;
		return z;
	}
	else return std::stod(node->value);
}

void printPrefix(Node* node) {
	std::cout << node->value << " ";
	if (node->right != NULL) {
		printPrefix(node->right);
	}
	if (node->left != NULL) {
		printPrefix(node->left);
	}

}

void Postfix(Node* node, std::vector<std::string>* v) {
	v->push_back(node->value);
	if (node->left != NULL) {
		Postfix(node->left, v);
	}
	if (node->right != NULL) {
		Postfix(node->right, v);
	}

}

void printPostfix(Node* node) {
	std::vector<std::string> vec = {};
	std::vector<std::string>* v = &vec;
	Postfix(node, v);
	for (int i = vec.size() - 1; i >= 0; i--) {
		std::cout << vec[i] << " ";
	}
}


int main()
{
	std::ifstream ifs("input.txt");
	std::string inputString = ReadExpression(ifs);
	//deleting spaces from the givem string
	inputString.erase(std::remove_if(inputString.begin(), inputString.end(), ::isspace), inputString.end());

	//checking if input string is valid
	if (true/*IsValid(inputString)*/) {
		std::cout << "Input string is valid" << std::endl;
		std::cout << "-----------------------------------------------\n";
		std::cout << "Your input: " << std::endl << inputString << std::endl;

		//stack for our nodes. Tree will be stored here
		std::stack<Node*> operands;
		//stack for operators - we need it to build the tree
		std::stack<Node*> operators;

		//BuildTree method returns processed tree as a stack of one node - our root
		operands = BuildTree(inputString, operands, operators);

		//calculated value of expression
		std::cout << "-----------------------------------------------\n";
		std::cout << "Value of expression is: " << EvaluateTree(operands.top()) << std::endl;
		std::cout << "-----------------------------------------------\n";

		//print prefix and postfix expressions
		std::cout << "Prefix expression:" << std::endl;
		printPrefix(operands.top());
		std::cout << "\n-----------------------------------------------\n";
		std::cout << "Postfix expression:" << std::endl;
		printPostfix(operands.top());
		std::cout << "\n-----------------------------------------------\n";
	}
	else {
		std::cout << "Input string is not valid" << std::endl;
		std::cout << "Your input: " << std::endl << inputString << std::endl;
	}

}