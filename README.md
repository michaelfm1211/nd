<style>
body {
	margin: 40px auto;
	max-width: 650px;
	line-height: 1.6;
	font-size: 18px;
	padding: 0 10px;
}
h1,h2,h3 {
	line-height: 1.2;
}
td {
	padding: 2 5px;
}
</style>

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
        <td>Magic (always `ND`)</td>
        <td>2</td>
    </tr>
    <tr>
        <td>Flags</td>
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

The flags field has 32 bits, each bit indicating a property of the data. The
below table shows the currently defined flags. Other bits may be used for
non-standard purposes, but there is no guarantee that they won't be assigned a
standard purpose at some point in the future. The names in the table below
correspond to names in the `libnd.h` header file.

<table>
    <th>
        <tr>
            <td>Name</td>
            <td>Indication</td>
            <td>Bit</td>
        </tr>
    </th>
    <tr>
        <td>`ND_FLAG_64BITS`</td>
        <td>Data records are 64 bits (8 bytes) long.</td>
        <td>0</td>
    </tr>
    <tr>
        <td>`ND_FLAG_UNSIGNED`</td>
        <td>Data represents unsigned integers.</td>
        <td>1</td>
    </tr>
</table>

The length header field is **optional**. If used, then it indicates the number
of data records (not the length in bytes of those records). If unused, then it
is recommended that it set to a value that makes it clear that it is unused,
such as zero or `INT64_MAX`, however, this is not required and it may be set to
anything. For this reason, **do not trust the length field**. If your code
blindly trusts the length field, then you are setting yourself up for a buffer
overflow.

Each data record is either 4 bytes long (32 bits) or 8 bytes long (64 bits). If
your data can fit into 4 bytes, then you should definitely use the 32 bit
option, as it will make your file about half as big. This is enough to fit a
regular `int`, `unsigned int`, or `float`. If you have to store large numbers
with precision (`int64`, `uint64`, or `double`) or want to store timestamps past
the [Epochalypse](https://en.wikipedia.org/wiki/Year_2038_problem), then use 64
bits. To indicate how long data records are, set or unset the `ND_FLAG_64BITS`
flag. If it is unset, then the data should be 4 bytes long. If it is set, then
the data should be 8 bytes long.

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
