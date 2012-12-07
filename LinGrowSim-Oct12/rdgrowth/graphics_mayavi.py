import numpy
from mayavi.mlab import *

#class Triangle:
    #def __init__(self, nodes, coordinates, concentrations):
		#self.nodes = nodes
        #self.coords = coordinates
        #self.concs = concentrations
        
#class Coords:
	#def __init__(self, x, y, z):
		#self.x = x
		#self.y = y
		#self.z = z
        
#class Concs:
	#def __init__(self, X, Y, A):
		#self.X = X
		#self.Y = Y
		#self.A = A

f = file('res091071/dump180','r')

all_data = f.read().split('\n')	# load dump file and separate the items

xyz_coords = {}
XYA_concs  = {}
neighbours = {}

number_of_nodes = int(all_data[13])
node_degree = int(all_data[14])
node_id = int(all_data[17])
node_id_coord = 17
for i in range(number_of_nodes-1):	# range over the number of nodes
	xyz_coords[node_id] = [float(x) for x in all_data[node_id_coord+1:node_id_coord+4]]
	XYA_concs[node_id] = [float(x) for x in all_data[node_id_coord+4:node_id_coord+7]]
	neighbours[node_id] = set([int(x) for x in all_data[node_id_coord+7:node_id_coord+7+node_degree]])
	#move to the next node
	node_id_coord	+= 10 + node_degree
	node_degree		= int(all_data[node_id_coord - 3])
	node_id 	   		= int(all_data[node_id_coord])
xyz_coords[node_id] = [float(x) for x in all_data[node_id_coord+1:node_id_coord+4]]
XYA_concs[node_id] = [float(x) for x in all_data[node_id_coord+4:node_id_coord+7]]
neighbours[node_id] = set([int(x) for x in all_data[node_id_coord+7:node_id_coord+7+node_degree]])
#this generates dictionaries for the coordinates, concentrations and
#neighbour lists. 

#Next, generate a list of all triangles
tri_nodes = []
for node1 in range(number_of_nodes):
	for node2 in neighbours[node1]:
		neighs = neighbours[node1].intersection(neighbours[node2])
		for node3 in neighs:
			if set([node1,node2,node3]) not in tri_nodes:
				tri_nodes.append(set([node1,node2,node3]))

#Next, generate a list each for x,y,z coordinates in order
xlist = []
ylist = []
zlist = []
for i in range(number_of_nodes):
	xlist.append(xyz_coords[i][0])
	ylist.append(xyz_coords[i][1])
	zlist.append(xyz_coords[i][2])
triangles_list = [list(i) for i in tri_nodes]

triangular_mesh(xlist,ylist,zlist,triangles_list)