Dear colleagues,
After the last changes in the Hbase table structure i believe it is time to define a basic test in order to evaluate the latest two designs, and see if splitting the information in several rows will affect the performance of the access queries.

i propose to implement this basic query: given the (x, y, z) of a point in space, return the coordinates of the vertices of the element that contains this point.
(This will also serve to implement the VQ202_0_GetResultForPoints Vquery)

i prepared a small FEM ascii mesh with 4 partitions that can be found at the acuario cluster /localfs/home/velassco/common/simulation_files
and in the alfresco platform at Documents >  Platform Design and Development >  HBase table format >  VELaSSCo_HbaseBasicTest_FEM https://alfresco.cimne.upc.edu/share/page/site/velassco/documentlibrary#filter=path|%2FPlatform%2520Design%2520and%2520Development%2FHBase%2520table%2520format%2FVELaSSCo_HbaseBasicTest_FEM|&page=1

The mesh is a cube subdivided into around 10,000 tetrahedrons and lie between (0, 0, 0) and ( 1, 1, 1), there are also some artificially created results but are optional for this test.

Inside the folder there is also a brief explanation of the GiD ASCII format which can also be found at http://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files

After injecting this data into the two latest hbase tables:

1. the one specified in HBase table format-Final-v5-with-VqueryResults.docx (https://alfresco.cimne.upc.edu/share/page/site/velassco/documentlibrary#filter=path|%2FPlatform%2520Design%2520and%2520Development%2FHBase%2520table%2520format|&page=1) 

2. the one with the latest modifications proposed by Miguel Angel and Ivan in the last technical teleconference ( 21st Mai) and in the teleco with Uedin, Atos and Cimne this Monday.

we'll evaluate how much time it takes on performing 1,000 vqueries with different (random?) coordinates between 0.0 and 1.1 ( so that some points will be outside the mesh).

In the Alfresco platform i've also uploaded an excel file to be filled with the numbers obtained on the tests, so that everyone can compare the results and even try new tests.

It is feasible to have some numbers for the next technical teleconference on the 3rd of June?

BasicTestQuery summary:
=======================
regarding the basic_test_query the query should:
- accept as input the (x,y,z) coordinates of one single vertex
- output as result the id of the element that contains the input vertex and the coordinates of their (4) vertices. Or nothing if the input vertex is not inside any element.

the measuring time will of of a loop over 1000 random points that will issue the vquery.
So, yes, we're also measuring the time of the 1000 vquery calls...

May be we can do some more detailed measurements, but i believe this is the one we should start.


IsPointInsideTetrahedron.cc
============================
the code to determine if a point is inside a tetrahedron is attached to this mail.
Given the coordinates of a point ( p_in_x, p_in_y, p_in_z) and the coordinates of the vertices (a, b, c, d) of a tetrahedron, it returns true if the point is inside the tetrahedron or at their faces or corners.

(a, b, c, d) define the tetrahedron using the right-hand rule, as in (http://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/mesh_format-_modelnamepostmsh)
That means that the first three points (a, b, c) define which half-space the fourth point is.
