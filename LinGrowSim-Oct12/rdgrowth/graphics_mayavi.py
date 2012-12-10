import numpy
from mayavi.mlab import *

#dump_directory = 'res091071/dump'
for i in range(5):
	dump_directory = '/home/irozada/Projects/Demos/6whorl_results/dump'
	dump_number = str(i)

	f = file(dump_directory + dump_number,'r')

	all_data = f.read().split('\n')	# load dump file and separate the items

	xyz_coords = {}
	XYA_concs  = {}
	neighbours = {}
	
	init_data_entries = 8 	#change to 9 if using data from res091071/..

	number_of_nodes = int(all_data[init_data_entries + 4])
	node_degree = int(all_data[init_data_entries + 5])
	node_id = int(all_data[init_data_entries + 8])
	node_id_coord = init_data_entries + 8
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

	Concentration_list_X = [XYA_concs[i][0] for i in range(number_of_nodes)]
	Concentration_list_Y = [XYA_concs[i][1] for i in range(number_of_nodes)]
	Concentration_list_A = [XYA_concs[i][2] for i in range(number_of_nodes)]

	triangular_mesh(xlist,ylist,zlist,triangles_list,scalars = Concentration_list_X)