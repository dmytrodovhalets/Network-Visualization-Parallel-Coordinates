#include <windows.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <iostream>
#include "glut.h"
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;
void Display();
void FlipDisplay();
void Init(void);
void displayHouse();

double w = 900.0;
double h = 680.0;
double R = w/h;
vector<int> network_origions;
int first_run_flag = 1;
int num_clicks = 0;
int networks_selected[2];

vector<double> readFile(string filename){
	fstream inStream;
	inStream.open(filename, ios::in);
	if(inStream.fail()){
		cout<<"FAILED TO GET THE FILE";
	} 
	double data;
	vector<double> data_array;
	while(inStream >> data){
		data_array.push_back(data);
	}
	inStream.close();
	return data_array;
}

//change file name for different datasets
//string filename = "dataset1.txt";
//string filename = "Dataset2.txt";
string filename = "Dataset3.txt";
vector<double> data_file = readFile(filename);

struct mousePointer {
	int x, y;
};

struct GLintPoint{
	GLint x, y;
};

struct origin{ // used to keep track of each networks location on GLC
		int x, y;
	};

void setWindow(float left, float right, float bottom, float top)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, right, bottom, top);
}


void setViewport(float left, float right, float bottom, float top)
{
	glViewport(left, bottom, right -left, top - bottom);
}


void drawDotY(int x, int y){
	glPointSize(8);
	glBegin(GL_POINTS); // render with points
	glColor3f(255.0/255, 255.0/255, 255.0/255);//white
		glVertex2i(x, y+20); //display a point 20 pading
	glEnd();
	glFlush();
}


void drawAxis(int numDimensions){//add x and y axis lines
	int num_dashes_y = 10; //Y axis ten needs to be changed
	int inBetweenX = w/numDimensions;
	int inBetweenY = h/10;

	glLineWidth(3.0);
	glColor3f(255.0/255, 255.0/255, 255.0/255);//white
	//glColor3f(0.0f,0.0f,0.0f);
	for(int i=0; i<numDimensions; i++){
		glBegin(GL_LINE_STRIP);//Y axis lines
			glVertex2i(i*inBetweenX+20, 0);
			glVertex2i(i*inBetweenX+20, h-40);
		glEnd();
	}

	for(int i=0; i<numDimensions; i++){//drawing dots
		for(int x=0; x<num_dashes_y; x++){
			drawDotY(i*inBetweenX+20, x*inBetweenY);
		}//closing inner loop
	}
	glFlush();
}


void drawNetworkNodes(vector<double> data, int num_networks){ //normolized number of nodes per network
	int inBetweenX = w/num_networks;
	int inBetweenY = h/11;
	vector<origin> origins;

	for(int i=0; i<num_networks; i++){
		glPointSize(15);
		if(i+1 == 1){
			glColor3f(0.0/255, 255.0/255, 0.0/255);//green
		}else if(i+1 == 2){
			glColor3f(255.0/255, 0.0/255, 0.0/255);//red
		}else if(i+1 == 3){
			glColor3f(9.0/255,38.0/255,249.0/255);//blue
		}else if(i+1 == 4){
			glColor3f(250.0/255, 200.0/255, 0.0/255);//yellow
		}else if(i+1 == 5){
			glColor3f(250.0/255, 0.0/255, 200.0/255);//pink
		}else if(i+1 == 6){
			glColor3f(0.0/255, 200.0/255, 200.0/255);//green + blue ?? lol
		}else if(i+1 == 7){
			glColor3f(100.0/255, 100.0/255, 100.0/255);//gray
		}else if(i+1 == 8){
			glColor3f(75.0/255, 31.0/255, 31.0/255);//brown
		}else if(i+1 == 9){
			glColor3f(19.0/255, 125.0/255, 22.0/255);//dark green
		}else if(i+1 == 10){
			glColor3f(243.0/255, 89.0/255, 42.0/255);//orange
		}

		glBegin(GL_POINTS); 
			glVertex2i(i*inBetweenX+20, (data[i]*inBetweenY+20)); //display a point 20 pading
		glEnd();
		glFlush();
		if(first_run_flag == 1){
			network_origions.push_back((i*inBetweenX+20+35));
			cout<<"x: "<<(i*inBetweenX+20+35)<<endl;
		}
	}
	first_run_flag = 99;// so it does not record the origions more than once
}


int howManyNetworks(){
	int num_networks = 0;
	int check = 0;
	int k = 0;
	for(int i=0; i<data_file.size()/3; i++){//3 columns in the data file
		for(int x=0; x<1; x++){
			int network_id = data_file[k+x]/1000;
			if(network_id > check){
				check = network_id;
				num_networks += 1;
			}
		}//closing inner loop
		k += 3;
	}
	return num_networks;
}


int howManyNodes(int check){
	int num_nodes = 0;
	int k = 0;
	for(int i=0; i<data_file.size()/3; i++){//3 columns in the data file
		int network_id = data_file[k];
		if(network_id >= check && network_id < check+1000){
			num_nodes += 1;
		}
		k += 3;
	}
	return num_nodes;
}


vector<double> normolizeNetworkSize(vector<int> nodes){
	double high = 0.0;
	double low = 100.0;
	for(int i=0; i<nodes.size(); i++){
		if(nodes[i] > high){
			high = nodes[i];
		}
		if(nodes[i] < low){
			low = nodes[i];
		}
	}//closing loop

	vector<double> normolized;
	for(int i=0; i<nodes.size(); i++){
		double normolized_node = ((nodes[i] - low)/(high - low))*10;
		normolized.push_back(normolized_node);
		//cout<<"nornolized "<<normolized_node<<endl;
	}
	return normolized;
}


vector<int> getSubColumn(int network_index, int flag){
	vector<int> num_nodes; //nodes per each network
	int num_networks = howManyNetworks();
	for(int i=0; i<num_networks; i++){
		int node_count = howManyNodes((i+1)*1000);
		num_nodes.push_back(node_count);
	}

	int start = 0;
	int stop = 0;
	int total = 0;
	for(int y=0; y<num_networks; y++){//getting the start and stop position of the other sub-column
		if(y == network_index){
			start = total;
			stop = total+ num_nodes[y];
		}
		total += num_nodes[y];
	}

	vector<int> data_vector;
	int m = 0;
	for(int j=0; j<data_file.size()/3; j++){//getting a vector of the sub_column
		if(j >= start && j < stop){
			if(flag == 1){
				data_vector.push_back(data_file[m]);
			}else{
				data_vector.push_back(data_file[m+1]);
			}
		}
		m += 3;
	}
	return data_vector;
}


int isNetworkConnected(int network_index1, int network_index2){
	vector<int> first_vector = getSubColumn(network_index1, 1);
	vector<int> second_vector = getSubColumn(network_index2, 2);

	int flag = 0;
	for(int a=0; a<first_vector.size(); a++){
		for(int u=0; u<second_vector.size(); u++){
			//checking if the node is connected from a different network
			if(first_vector[a]/1000 == second_vector[u]/1000){
				flag = 1;
			}
		}
	}
	return flag;
}


void drawConnections(int num_networks){
	int inBetweenX = w/num_networks;
	int inBetweenY = h/11;

	vector<int> num_nodes; //nodes per each network
	for(int i=0; i<num_networks; i++){
		int node_count = howManyNodes((i+1)*1000);
		num_nodes.push_back(node_count);
	}
	vector<double> normolized_network_size = normolizeNetworkSize(num_nodes);
	glLineWidth(2.0);
	for(int i=0; i<num_networks; i++){
		for(int k=0; k<num_networks; k++){
			int connected = isNetworkConnected((i), (k));
			if(connected == 1){
				cout<<(i+1)*1000<<" is conneceted to "<<(k+1)*1000<<endl;
				glColor3f(0.0f,0.0f,0.0f);
				glBegin(GL_LINE_STRIP);//Y axis lines
					glVertex2i(i*inBetweenX+20, normolized_network_size[i]*inBetweenY+20);
					glVertex2i(k*inBetweenX+20, normolized_network_size[k]*inBetweenY+20);
				glEnd();
				
			}
		}//clossing inner loop
	}//closing outer loop
	glFlush();
}


int isNodeConnected(int node1, int node2){
	return 0;
}

//origionla screen
void drawNetworks(){
	int num_networks = howManyNetworks();
	drawAxis(num_networks);
	vector<int> num_nodes; //nodes per each network

	for(int i=0; i<num_networks; i++){
		int node_count = howManyNodes((i+1)*1000);
		num_nodes.push_back(node_count);
	}

	vector<double> normolized_network_size = normolizeNetworkSize(num_nodes);
	drawNetworkNodes(normolized_network_size, num_networks);
	drawConnections(num_networks);
	//vector<int> pointVertices(nomral
}


vector<double> getValuesColumn3(int network_index){
	vector<int> num_nodes; //nodes per each network
	int num_networks = howManyNetworks();
	for(int i=0; i<num_networks; i++){
		int node_count = howManyNodes((i+1)*1000);
		num_nodes.push_back(node_count);
	}

	int start = 0;
	int stop = 0;
	int total = 0;
	for(int y=0; y<num_networks; y++){//getting the start and stop position of the other sub-column
		if(y == network_index){
			start = total;
			stop = total+ num_nodes[y];
		}
		total += num_nodes[y];
	}

	vector<double> data_vector;
	int m = 0;
	for(int j=0; j<data_file.size()/3; j++){//getting a vector of the sub_column
		if(j >= start && j < stop){
			data_vector.push_back(data_file[m+2] * 10);
			cout<<"values: "<<data_file[m+2]<<endl;
		}
		m += 3;
	}
	return data_vector;
}


int getValueForConnectingNode(int node_id, vector<int> column){
	int node_index =0;
	for(int i=0; i< column.size(); i++){
		if(column[i] == node_id){
			node_index = i;
		}
	}
	return node_index;
}


void drawConnectionsSingleNetwork(int num_nodes, int network_id){
	int inBetweenX = w/num_nodes;
	int inBetweenY = h/11;
	vector<double> normolized_network_size = getValuesColumn3(network_id);
	
	vector<int> column1 = getSubColumn(network_id, 1);
	vector<int> column2 = getSubColumn(network_id, 2);
	glLineWidth(2.0);
	//check each node against each other to see if they are connected.
	for(int i=0; i<column1.size(); i++){
		glColor3f(0.0f,0.0f,0.0f);
		glBegin(GL_LINE_STRIP);//Y axis lines
			glVertex2i(i*inBetweenX+20, normolized_network_size[i]*inBetweenY+20);

			//getting the nodes value to find out where to connect
			int node_id = getValueForConnectingNode(column2[i], column1);

			glVertex2i(node_id*inBetweenX+20, normolized_network_size[node_id]*inBetweenY+20);
		glEnd();
	}//closing loop
	glFlush();
}



void drawNodesSingleNetwork(vector<double> data, int network_id){ //normolized number of nodes per network
	int inBetweenX = w/data.size();
	int inBetweenY = h/11;
	int i = network_id;
	vector<int> column2 = getSubColumn(network_id, 2);
	for(int k=0; k<data.size(); k++){
		glBegin(GL_POINTS); 
		//select color based on the network connection
		i = column2[k]/1000;

		if(i == 1){
			glColor3f(0.0/255, 255.0/255, 0.0/255);//green
		}else if(i == 2){
			glColor3f(255.0/255, 0.0/255, 0.0/255);//red
		}else if(i == 3){
			glColor3f(9.0/255,38.0/255,249.0/255);//blue
		}else if(i == 4){
			glColor3f(250.0/255, 200.0/255, 0.0/255);//yellow
		}else if(i == 5){
			glColor3f(250.0/255, 0.0/255, 200.0/255);//pink
		}else if(i == 6){
			glColor3f(0.0/255, 200.0/255, 200.0/255);//green + blue ?? lol
		}else if(i == 7){
			glColor3f(100.0/255, 100.0/255, 100.0/255);//gray
		}else if(i == 8){
			glColor3f(75.0/255, 31.0/255, 31.0/255);//brown
		}else if(i == 9){
			glColor3f(19.0/255, 125.0/255, 22.0/255);//dark green
		}else if(i == 10){
			glColor3f(243.0/255, 89.0/255, 42.0/255);//orange
		}

			glVertex2i(k*inBetweenX+20, (data[k]*inBetweenY+20)); //display a point 20 pading
		glEnd();
		glFlush();
	}

	
}

//main for single network
void drawSingleNetwork(int network_id){
	glClear(GL_COLOR_BUFFER_BIT);
	int num_networks = howManyNetworks();
	vector<int> num_nodes; //nodes per each network

	for(int i=0; i<num_networks; i++){
		int node_count = howManyNodes((i+1)*1000);
		num_nodes.push_back(node_count);
	}
	//draw new axis based on number of nodes in network_id
	drawAxis(num_nodes[network_id]);

	//get column with doubles
	vector<double> column = getValuesColumn3(network_id);
	drawNodesSingleNetwork(column, network_id);

	//drawConnections
	drawConnectionsSingleNetwork(column.size(), network_id);
}


void drawTwoNetworks(int network_id_1, int network_id_2){
	glClear(GL_COLOR_BUFFER_BIT);
	//set view port
	glViewport(10, 100,  (w/2)-10 , h - 200);
	int num_networks = howManyNetworks();
	vector<int> num_nodes; //nodes per each network

	for(int i=0; i<num_networks; i++){
		int node_count = howManyNodes((i+1)*1000);
		num_nodes.push_back(node_count);
	}
	//draw new axis based on number of nodes in network_id
	drawAxis(num_nodes[network_id_1]);
	//get column with doubles
	vector<double> column = getValuesColumn3(network_id_1);
	drawNodesSingleNetwork(column, network_id_1);
	//drawConnections
	drawConnectionsSingleNetwork(column.size(), network_id_1);

	glViewport(0, 0,  w, h);
	glBegin(GL_LINE_STRIP);
	glVertex2i(w/2, 0);
	glVertex2i(w/2, h);
	glEnd();
	glFlush();

	//set view port
	glViewport((w/2)+10, 100,  w-450, h - 200);
	num_networks = howManyNetworks();
	vector<int> num_nodes_x; //nodes per each network

	for(int i=0; i<num_networks; i++){
		int node_count = howManyNodes((i+1)*1000);
		num_nodes_x.push_back(node_count);
	}
	//draw new axis based on number of nodes in network_id
	drawAxis(num_nodes_x[network_id_2]);
	//get column with doubles
	vector<double> column_x = getValuesColumn3(network_id_2);
	drawNodesSingleNetwork(column_x, network_id_2);
	//drawConnections
	drawConnectionsSingleNetwork(column_x.size(), network_id_2);
}



void mouseClick(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {

		//goes through the origons of networks to see if its close
		for(int i=0; i<network_origions.size(); i++){
			if(x >= network_origions[i]-35 && x <= network_origions[i]+35){
				cout<<"selected network num: "<< i+1<<endl;
				num_clicks += 1;

				//checks for first or second click from the user
				if(num_clicks == 1){
					networks_selected[0] = i;
				}else{
					networks_selected[1] = i;
				}

				//checks for double click
				if(num_clicks == 2){
					num_clicks = 0;

					//checking if one network should be drawn or two side by side
					if(networks_selected[0] == networks_selected[1]){
						//draw one network
						cout<<"draw network: "<< networks_selected[0];
						drawSingleNetwork(networks_selected[0]);
					}else{
						//draw two networks
						cout<<"draw network: "<< networks_selected[0]<<" and "<< networks_selected[1]<<endl;
						drawTwoNetworks(networks_selected[0], networks_selected[1]);
					}
				}
			}
		}//end for
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {//reset to networks
		cout<<"reset"<<endl;
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(60, 60,  w-60, h - 60);
		drawNetworks();
	}
}


void Display(){
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(60, 60,  w-60, h - 60);
	drawNetworks();
}

void Init()
{
	glClearColor(154/255.0, 150/255.0 ,150/255.0, 1.0);
   glColor3f(0.0f,0.0f,0.0f);
   glPointSize(4.0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0,w,0.0,h);
}


void main (int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(w, h);
	//glViewport(0, 640, 0, 480);
	glutInitWindowPosition(100,150);
	glutCreateWindow("Final Project: Dmytro, Dan, Alex");
	glutDisplayFunc(Display);
	glutMouseFunc(mouseClick);
    Init();
	glutMainLoop();
}