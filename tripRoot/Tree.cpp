#include "Tree.h"

Node::Node(){
	this->label = "";
	this->edge = 0;
}

Node::Node(string label){
	this->label = label;
	this->edge = 0;
}

Node::~Node(){
	for (int i = 0; i < this->children.size(); i++)
		delete this->children[i];
}

string Node::get_label(){
	return this->label;
}

bool Node::set_label(string s){
	this->label = s;
	return true;
}

int Node::nchild(){
	return this->children.size();
}

Node* Node::get_child(int n){
	Node* ch = NULL;
	if (n >= 0 && n < this->children.size())
		ch = this->children[n];
	return ch;
}

bool Node::add_child(Node *ch){
	this->children.push_back(ch);
	return true;
}

double Node::get_edge(){
	return this->edge;
}

bool Node::set_edge(double e){
	this->edge = e;
	return true;
}

bool Node::writeNewick(ofstream &fout){
	if (!this->children.empty())
	{
		fout << "(";
		int n = this->children.size();
		this->children[n-1]->writeNewick(fout);
		for (int i = n-2; i >= 0; i--){
			fout << ",";
			this->children[i]->writeNewick(fout);
		}
		fout << ")";
	}
	fout << this->get_label() << ":" << this->get_edge();
}

Tree::Tree(){
	root = NULL;
}

Tree::~Tree(){
	delete root;
}

/*Node* Tree::create_node(){
	cout << "Created a normal node" << endl;
	Node *p = new Node;
	return p;
}*/

bool Tree::readNewick(string treefile){
		char c;
		bool wait_for_int_lab = false;
		stack<Node*> stk;

		string label = "";
		ifstream fin;
		fin.open(treefile);
		while (!fin.eof()){
			fin >> c;
			if (c == '('){
				stk.push(NULL);
				label = "";
			}
			else if (c == ',')  {
				if (label != ""){
					if (!wait_for_int_lab){
						//Node *p = new Node;
						Node *p = this->create_node();
						p->set_label(label);
						stk.push(p);
					} else {
						stk.top()->set_label(label);
					}
					label = "";
				}
				label = "";
				wait_for_int_lab = false;
			} else if (c == ')'){
				if (label != ""){
					if (!wait_for_int_lab){
						//Node *p = new Node;
						Node *p = this->create_node();
						p->set_label(label);
						stk.push(p);
					} else {
						stk.top()->set_label(label);
					}
					label = "";
				}
				//Node *p = new Node;
				Node *p = this->create_node();
				Node *q;
				while (1){
					q = stk.top();
					stk.pop();
					if (q)
					{
						//cout << "I pushed back something!" << endl;
						//p->children.push_back(q);
						p->add_child(q);
					}
					else
						break;
				}
				stk.push(p);
				label = "";
				wait_for_int_lab = true;
			} else if (c == ';'){
				this->root = stk.top();
				//this->root->set_label("root");
				stk.pop();
				break;
			} else if (c == ':'){
				if (label != ""){
					if (!wait_for_int_lab){
						//Node *p = new Node;
						Node *p = this->create_node();
						p->set_label(label);
						stk.push(p);
					} else {
						stk.top()->set_label(label);
					}
					label = "";
				}
				double e;
				fin >> e;
				stk.top()->set_edge(e);
				wait_for_int_lab = false;
			}
			else {
				label += c;
			}
		}
		fin.close();
		return true;
	}
	
bool Tree::writeNewick(string treefile){
		ofstream fout;
		fout.open(treefile);
		//this->root->set_label("root");
		//fout << this->root->get_label();
		this->root->writeNewick(fout);
		fout << ";";
		fout.close();
	}
/*
int main(){
	Tree a_tree;
	a_tree.readNewick("avian_fixed.tre");
	a_tree.writeNewick("out.tre");
}*/
