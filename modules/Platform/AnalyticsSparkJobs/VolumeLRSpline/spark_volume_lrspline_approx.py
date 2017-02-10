#!/usr/bin/python
#
# Run the volume lrspline approximation.
#

import math
import sys
import re
import os

import struct

#from bitarray import bitarray

import csv

import pickle

import io

import volume_lrspline_approx_ext

from pyspark import SparkContext, SparkConf

from array import *

def tobits(s):
    result = []
    for c in s:
        bits = bin(ord(c))[2:]
        bits = '00000000'[len(bits):] + bits
        result.extend([int(b) for b in bits])
    return result

def max(a, b):
    if a > b:
        return a
    else:
        return b

def min(a, b):
    if a < b:
        return a
    else:
        return b

# Dummy function for testing.
def readLines(lines):
    list_length = len(lines)
    print "list_length:", list_length
    first_line = lines[0]
    return list_length

# Assuming 1D field.
def sumOverField(pt,
                 field_ind):
    pt_list = pt.split()
    len_pt_list = len(pt_list)
    if field_ind >= len_pt_list:
        print "WARNING: sumOverField: Too few vals for pt: ", pt
        return 0.0

    value = float(pt_list[field_ind])
    return value
    
# Assuming 1D field.
def tolStdDev(pt,
              data_mean,
              field_ind):
    pt_list = pt.split()
    len_pt_list = len(pt_list)
    if field_ind >= len_pt_list:
        print "WARNING: tolStdDev: Too few vals for pt: ", pt
        return 0.0

    # If the tolerance has not been set then compute it from the standard deviation
    dist_mean = data_mean - float(pt_list[field_ind])
    data_sd = pow(dist_mean,2)
    return data_sd

# Our map function. Returns a string containing the result (to be written to file).
def volumeLRSplineApproxPtList(lines,
                               ref_level_all_dim, # Typically a multiple of 3 for 3-dim geometry space).
                               field_first_comp, # Typically 4 (1-3 are velocity xyz).
                               field_last_comp, # Typically 4 (for 1-dimensional field, length of velocity vector).
                               tolerance):
    list_length = len(lines)
    print "INFO: Python: Number of points: ", list_length
    print "INFO: Python: ref_level_all_dim: ", ref_level_all_dim
    print "INFO: Python: field_first_comp: ", field_first_comp
    print "INFO: Python: field_last_comp: ", field_last_comp
    print "INFO: Python: tolerance: ", tolerance

    try:

        # Typical values for params ref_level_all_dim, field and tol: 3, 4, 4, -1.0,
        result_bin_string = volume_lrspline_approx_ext.volume_lrspline_approx_pt_list(lines,
                                                                                      ref_level_all_dim,
                                                                                      field_first_comp,
                                                                                      field_last_comp,
                                                                                      tolerance)
        #print "num_len(result_bin_string): ", len(result_bin_string)
        return result_bin_string
    except:
        return "ERROR: Failed!"


if __name__ == "__main__":

    if len(sys.argv) != 7:
        print "Usage: <input_pts.txt> <ref_level_all_dim> <field_first_comp> <field_last_comp> <tolerance> <filein_sf.g2> "
        sys.exit(1)

    input_pts_filename = sys.argv[1]
    ref_level_all_dim = int(sys.argv[2])
    field_first_comp = int(sys.argv[3])
    field_last_comp = int(sys.argv[4])
    tolerance = float(sys.argv[5])
    result_filename = sys.argv[6]

    # We configure Spark. The cluster URL (setMaster) should be given as a parameter to spark-submit.
    conf = SparkConf().setAppName("PythonSparkVolumeLRSplineApprox")
    sc = SparkContext(conf=conf)
 
    # We fetch the pointcloud filenames (and content) from a dir, store them in a RDD.
    #set_num_part = 20 # This is a lower boundary for the number of partitions, spark may increase the value.
    rdd_pts_data = sc.textFile(input_pts_filename)#, set_num_part)
    
    # We remove the header
    header = rdd_pts_data.first()
    rdd_pts_data = rdd_pts_data.filter(lambda row : row != header)
    num_pts = rdd_pts_data.count()
    print "INFO: Number of elements in rdd_pts_data: ", num_pts
    print "INFO: Number of partitions: ", rdd_pts_data.getNumPartitions()

    module_version = volume_lrspline_approx_ext.get_version()
    print "INFO: module_version: ", module_version

    if tolerance < 0:
        if num_pts > 20000000:
            print "INFO: Enabling spark tolerance computation."
            if field_first_ind != field_last_ind:
                print "Update tolerance spark tolerance computation with 3d field!"
            field_first_ind = 2 + field_last_comp
            field_sum = rdd_pts_data.map(lambda pt: sumOverField(pt, field_first_ind)).sum()
            field_avg = field_sum/num_pts

            tol_sd = rdd_pts_data.map(lambda pt: tolStdDev(pt, field_avg, field_first_ind)).sum()
            tol_sd_avg = tol_sd/num_pts
            tol = math.sqrt(tol_sd_avg)
            print "tol: ", tol

            tolerance = tol
        else:
            print "INFO: Negative input tolerance, but too few points to enable spark tolerance computation."
    
    # coalesce(1): Gather all data on 1 partition. glom(): Gather all data elements (lines) in a list.
    # map(...): Define transformation function for all the data elements. collect(): Fetch result (run map function).
    result = rdd_pts_data.coalesce(1).glom().map(lambda lines: volumeLRSplineApproxPtList(lines,
                                                                                          ref_level_all_dim,
                                                                                          field_first_comp,
                                                                                          field_last_comp,
                                                                                          tolerance)).collect()
    #    line = rdd_pts_data.glom().foreach(lambda lines: readLines(lines)).reduce(min)
    print "INFO: The RDD has finished!"

    # The result is a list.
    # print "len(result): ", len(result)
    # print "len(result[0]): ", len(result[0])
    # print "Writing result to: ", result_filename
    volume_lrspline_approx_ext.write_binary_result(result[0], result_filename)
    
    print "INFO: Done with spark_volume_lrspline_approx!"
