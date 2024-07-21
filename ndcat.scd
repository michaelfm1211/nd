ndcat(1)

# NAME

ndcat - dump the contents of a ND file

# SYNOPSIS

ndcat [-hilx] [-r range_start[,range_end]] file

# OPTIONS

*-h*
	Print information about the file from the header and exit.

*-l*
	Print the index of each data point.

*-x*
	Print data as hexadecimal integer, regardless of the data type
	indicated in the header.

*-r*
	Only read a range of data points. If no end range is given, then only
	one data point is printed.

# DESCRIPTION

ndcat reads a ND (numerical data) file and prints the data points in the file.

# SEE ALSO

*scdoc*(5)
