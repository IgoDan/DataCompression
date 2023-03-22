# Introduction
Performs Huffman Coding to losslessly compress and then decompress text files.

# Steps
- Data source model from data file
- Coding tree from data source model
- Code table from coding tree
- Compression using code table
- Decompression using code table

![image](https://user-images.githubusercontent.com/87280929/226876463-af2e9f31-fa42-492f-9fad-1222518a7d51.png)

# Compression

- Original file -> Size 149B:

![image](https://user-images.githubusercontent.com/87280929/226871969-022e2fc4-6c2d-41c9-a271-f6e2ee8d7130.png)

- Compressed file (in HEX) -> Size 81B:

![image](https://user-images.githubusercontent.com/87280929/226872346-16e120d2-d8d9-469e-996b-7d6d85f617a6.png)

- Using code table:

![image](https://user-images.githubusercontent.com/87280929/226873413-4d595f39-a649-417a-84d8-260f361b35fc.png)

# Decompression

Reading sequence of bits in compressed file and decoding characters using code table.

- Compressed file (in HEX) -> Size 81B:

![image](https://user-images.githubusercontent.com/87280929/226872346-16e120d2-d8d9-469e-996b-7d6d85f617a6.png)

- Decompressed file -> Size 149B:

![image](https://user-images.githubusercontent.com/87280929/226871969-022e2fc4-6c2d-41c9-a271-f6e2ee8d7130.png)

- Console view:

![image](https://user-images.githubusercontent.com/87280929/226876612-807050d4-2a07-4f99-bc7c-130b723c2832.png)

# Compression for long files

Size of compressed file in comparison to original file gets much smaller alongside with increasing file size.

For example:

- Original file -> 2,72 MB
- Compressed file -> 1,63 MB



