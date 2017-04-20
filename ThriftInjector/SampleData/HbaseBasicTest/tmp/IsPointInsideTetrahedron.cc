/* -*- c++ -*- */

// point inside first tetrahedron of the VELaSSCo_HbaseBasicTest example
// is 0.034967938,0.689002752,0.025866667
// it is the centre of the first tetrahedron
// assuming numbering starts with 1, then:
// first tetrahedron ( id = 1) has nodes = 1794 1660 1802 1768
// in c/c++, numberations starts with 0, so nodes = 1793 1659 1801 1767
// coordinates of these nodes are:
//    1660 0.07139438390731812 0.7269042730331421 0.0
//    1768 0.0 0.683978796005249 0.10346666723489761
//    1794 0.06847736984491348 0.6451279520988464 0.0
//    1802 0.0 0.699999988079071 0.0

bool IsPointInsideTetrahedron( double p_in_x, double p_in_y, double p_in_z,
                               double a_x, double a_y, double a_z,
                               double b_x, double b_y, double b_z,
                               double c_x, double c_y, double c_z,
                               double d_x, double d_y, double d_z,
                               double epsilon) {
  double r, s, t;
  r = s = t = 0;

  double da_x = a_x - d_x;
  double da_y = a_y - d_y;
  double da_z = a_z - d_z;
  double db_x = b_x - d_x;
  double db_y = b_y - d_y;
  double db_z = b_z - d_z;
  double dc_x = c_x - d_x;
  double dc_y = c_y - d_y;
  double dc_z = c_z - d_z;
  double dp_x = p_in_x - d_x;
  double dp_y = p_in_y - d_y;
  double dp_z = p_in_z - d_z;
  double r_denominator = da_x * ( db_y * dc_z - dc_y * db_z) +
      db_x * ( dc_y * da_z - da_y * dc_z) +
      dc_x * ( da_y * db_z - db_y * da_z);
  double r_numerator = dp_x * ( db_y * dc_z - dc_y * db_z) +
      db_x * ( dc_y * dp_z - dp_y * dc_z) +
      dc_x * ( dp_y * db_z - db_y * dp_z);

  if ( fabs( r_numerator) < epsilon)
    r_numerator = 0.0f;
  if ( fabs( r_denominator) > epsilon)
    r = r_numerator / r_denominator;
  else
    r = 0.0f;
  if ( ( r < 0.0f) || ( r > 1.0f))
    return false;

  double s_denominator = db_y * dc_z - dc_y * db_z;
  double s_numerator = dp_y * dc_z - dc_y * dp_z + r * ( dc_y * da_z - da_y * dc_z);
  if ( fabs( s_numerator) < epsilon)
    s_numerator = 0.0f;
  if ( fabs( s_denominator) > epsilon)
    s = s_numerator / s_denominator;
  else {
    s_denominator = db_y * dc_x - dc_y * db_x;
    s_numerator = dp_y * dc_x - dc_y * dp_x + r * ( dc_y * da_x - da_y * dc_x);
    if ( fabs( s_denominator) > epsilon)
      s = s_numerator / s_denominator;
    else {
      s_denominator = db_x * dc_z - dc_x * db_z;
      s_numerator = dp_x * dc_z - dc_x * dp_z + r * ( dc_x * da_z - da_x * dc_z);
      if ( fabs( s_denominator) > epsilon)
        s = s_numerator / s_denominator;
      else {
        s = 0.0f;
      }
    }
  }
  if ( ( s < 0.0f) || ( s > 1.0f))
    return false;

  double t_denominator = dc_z;
  double t_numerator = dp_z - r * da_z - s * db_z;
  if ( fabs( t_numerator) < epsilon)
    t_numerator = 0.0f;
  if ( fabs( t_denominator) > epsilon)
    t = t_numerator / t_denominator;
  else {
    t_denominator = dc_x;
    t_numerator = dp_x - r * da_x - s * db_x;
    if ( fabs( t_denominator) > epsilon)
      t = t_numerator / t_denominator;
    else {
      t_denominator = dc_y;
      t_numerator = dp_y - r * da_y - s * db_y;
      if ( fabs( t_denominator) > epsilon)
        t = t_numerator / t_denominator;
      else {
        t = 0.0f;
      }
    }
  }
  if ( ( t < 0.0f) || ( t > 1.0f))
    return false;

  if ( ( ( r + s + t) >= -0.0) && ( ( r + s + t) <= 1.0f)) {
    return true;
  } else {
    return false;
  }
}
