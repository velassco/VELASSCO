/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package GetBoundaryOfAMesh;

import static GetBoundaryOfAMesh.GetBoundaryOfAMesh.toHexString;
import static GetBoundaryOfAMesh.GetBoundaryOfAMesh.fromHexString;
import static GetBoundaryOfAMesh.GetBoundaryOfAMesh.println;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.hbase.util.Bytes;

/**
 *
 * @author icores
 */

public class MR_BoundaryTriangle implements WritableComparable< MR_BoundaryTriangle> {
    long _n1, _n2, _n3;
    long _n1_org, _n2_org, _n3_org;
    // static int _num_nodes = 3;
    
    public MR_BoundaryTriangle() {
        _n1 = 0; _n2 = 0; _n3 = 0;
        _n1_org = 0; _n2_org = 0; _n3_org = 0;
    }
    public MR_BoundaryTriangle( long n1, long n2, long n3) {
        set( n1, n2, n3);
    }
    public void set( long n1, long n2, long n3) {
        _n1_org = n1; _n2_org = n2; _n3_org = n3;
        long min = Math.min( n1, Math.min( n2, n3));
        long max = Math.max( n1, Math.max( n2, n3));
        long sum = n1 + n2 + n3;
        _n1 = min; _n3 = max;
        _n2 = sum - min - max;
    }
    
    public long n1() {
        return _n1;
        // return Math.min( _n1_org, Math.min( _n2_org, _n3_org));
    }
    public long n2() {
        return _n2;
        // long min = Math.min( _n1_org, Math.min( _n2_org, _n3_org));
        // long max = Math.max( _n1_org, Math.max( _n2_org, _n3_org));
        // return _n1_org + _n2_org + _n3_org - min - max;
    }
    public long n3() {
        return _n3;
        // return Math.max( _n1_org, Math.max( _n2_org, _n3_org));
    }

    public boolean fromString( String str) {
        byte[] data = fromHexString( str);
        
        int num_nodes = Bytes.toInt( data, 0);
        int nbytes_to_read = num_nodes * 8; // 8 = sizeof( Long)
        if ( num_nodes !=3) {
            println( "Reading triangle: num_nodes != 3 = " + num_nodes);
            println( "hexadecimal = " + str);
            println( "bytes length = " + data.length);
            return false;
        }
        long n1 = Bytes.toLong( data,  4);
        long n2 = Bytes.toLong( data, 12);
        long n3 = Bytes.toLong( data, 20);
        this.set(n1, n2, n3);
        return true;
    }
    
    // needed by the TextOutputFormat class used as output in the Reducer. --> should try binary !!!
    @Override
    public String toString() {
        // need to preserve original triangle orientation to have a coherent normal orientation on the skins ...
        // ascii:
        // return Long.toString( _n1_org) + " " + Long.toString( _n2_org) + " " + Long.toString( _n3_org);
        // hexadecimal text:
        // return GetBoundaryOfAMesh.toHexString( Bytes.toBytes( _n1_org)) + 
        //         GetBoundaryOfAMesh.toHexString( Bytes.toBytes( _n2_org)) + 
        //         GetBoundaryOfAMesh.toHexString( Bytes.toBytes( _n3_org)); 
        // binary output:
        // first the number of nodes, then the nodes themselves
        // number of nodes is for the case of prisms where
        // there will be triangles and quadrilaterals as boundary faces.

        return toHexString( Bytes.toBytes( 3)) + // _num_nodes)) +
                toHexString( Bytes.toBytes( _n1_org)) + 
                toHexString( Bytes.toBytes( _n2_org)) + 
                toHexString( Bytes.toBytes( _n3_org));
    }
    
    // Needed by WritableComparable:
    // write(), readFields() and compareTo()
    @Override
    public void write(DataOutput out) throws IOException {
        out.writeLong( _n1_org);
        out.writeLong( _n2_org);
        out.writeLong( _n3_org);
    }

    @Override
    public void readFields(DataInput in) throws IOException {
        long n1 = in.readLong();
        long n2 = in.readLong();
        long n3 = in.readLong();
        this.set(n1, n2, n3);
    }

    @Override
    public int compareTo(MR_BoundaryTriangle tri) {
        int ret = Long.compare( n1(), tri.n1());
        if ( ret != 0) return ret;
        else {
            ret = Long.compare( n2(), tri.n2());
            if ( ret != 0) return ret;
            else {
                ret = Long.compare( n3(), tri.n3());
                return ret;
            }
        }
    }

    // Needed by HashMap & Collections & ...
    // hashCode() and equals()
    @Override
    public int hashCode() {
        // should be the same as used in euqals() and compareTo()
        // long hash = 2965891 * _n1_org + 759569 * _n2_org + 32429 * _n3_org;
        long hash = 2965891 * n1() + 759569 * n2() + 32429 * n3();
        return ( int)( hash & 0xffffffff) ^ ( int)( ( hash >> 32) & 0xffffffff);
    }
    
    @Override
    public boolean equals( Object obj) {
        boolean ret = false;
        if ( obj instanceof MR_BoundaryTriangle) {
            MR_BoundaryTriangle tri = ( MR_BoundaryTriangle)obj;
            if ( ( n1() == tri.n1()) && ( n2() == tri.n2()) && ( n3() == tri.n3()))
                ret = true;
        }
        return ret;
    }
}

