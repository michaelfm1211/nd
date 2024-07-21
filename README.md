# nd

ND is a super simple binary format for compact numerical data. Each file can
contain one column of 32 bit or 64 bit numbers, either signed or unsigned, and
an arbitrary number of rows.

Implementing it can easily done with just `fread()` and `fwrite()` (or the
equivalent standard library procedures in your language). This repo provides a
very simple static library (libnd) to read the header for you and lets you parse
the data.

### Format

ND files have this format:
<table>
    <th>
        <tr>
            <td>Name</td>
            <td>Length (bytes)</td>
        </tr>
    </th>
    <tr>
        <td>Magic (always "ND")</td>
        <td>2</td>
    </tr>
    <tr>
        <td>Type (see table below)</td>
        <td>4</td>
    </tr>
    <tr>
        <td>Length</td>
        <td>8</td>
    </tr>
    <tr>
        <td>Data Record 1</td>
        <td>4 or 8</td>
    </tr>
    <tr>
        <td>Data Record 2</td>
        <td>4 or 8</td>
    </tr>
    <tr>
        <td>...</td>
        <td>...</td>
    </tr>
</table>

All fields must be in little endian.

The first three fields (magic, flags, and length) are the header and are
collectively 14 bytes. The magic must be the ASCII characters `ND` (`0x4e44`)
for the file to be identified as a ND file.

The type field has 32 bits. Types with an even ID are 64 bits long, while types
with an odd ID are 32 bits. The below table shows the currently defined types.
Other IDs may be used for non-standard purposes, but there is no guarantee that
they won't be assigned a standard purpose at some point in the future. The
names in the table below correspond to names in the `libnd.h` header file.

<table>
    <th>
        <tr>
            <td>Name</td>
            <td>Description</td>
            <td>ID</td>
        </tr>
    </th>
    <tr>
        <td>ND_TYPE_INT64</td>
        <td>64-bit signed integer.</td>
        <td>0</td>
    </tr>
    <tr>
        <td>ND_TYPE_INT32</td>
        <td>32-bit signed integer.</td>
        <td>1</td>
    </tr>
    <tr>
        <td>ND_TYPE_UINT64</td>
        <td>64-bit unsigned integer.</td>
        <td>2</td>
    </tr>
    <tr>
        <td>ND_TYPE_UINT32</td>
        <td>32-bit unsigned integer.</td>
        <td>3</td>
    </tr>
    <tr>
        <td>ND_TYPE_FLOAT64</td>
        <td>IEEE754 double precision floating point number.</td>
        <td>4</td>
    </tr>
    <tr>
        <td>ND_TYPE_FLOAT32</td>
        <td>IEEE754 single precision floating point number.</td>
        <td>5</td>
    </tr>
    <tr>
        <td>ND_TYPE_DATETIME</td>
        <td>UNIX timestamp (64-bits).</td>
        <td>6</td>
    </tr>
</table>

The length header field is **optional**. If used, then it indicates the number
of data records (not the length in bytes of those records). If unused, then it
is recommended that it set to a value that makes it clear that it is unused,
such as zero or `INT64_MAX`, however, this is not required and it may be set to
anything. For this reason, **do not trust the length field**. If your code
blindly trusts the length field, then you are setting yourself up for a buffer
overflow.

### Why ND?

ND is a very compact format, so it takes up less spaces. Even less if it is
stored compressed. Compared to CSV, it is a binary format and so saves a lot of
space by using 4 or 8 byte numbers rather than strings that are 1 byte per
character. Also, there is almost no need for parsing: just read numbers
sequentially.

Unlike CSV, ND only really has one column of data and an arbitrary number of
rows. To store data with multiple columns, like an (x, y) coordinate pair, you
must get creative. You could just store the data by storing two rows at a time,
effectively interlacing two columns into one, then read it back two columns at
a time. You could also split the x data into one file and the y data into
another file, then read from two files simultaneously.
