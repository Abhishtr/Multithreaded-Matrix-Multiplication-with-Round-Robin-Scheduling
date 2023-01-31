import numpy as np
import sys
LONG_LONG_MAX =  9223372036854775807

def genMatrix():
    #size = input("Enter nxm matrix size:")
    #size = size.split("x")

    n = int(sys.argv[1])
    m = int(sys.argv[2])
    #f =  open("Matrix_out.txt")
    mat = np.random.randint(0,1000,size=(n,m))
    #print(mat)
    #np.savetxt("matrix1_OF.txt",mat,fmt='%i')
    np.savetxt(sys.argv[4],mat,fmt='%i');

    #size2 = input("Enter second matrix size:")
    #size2 = size2.split("x")
    m2 = int(sys.argv[3])

    mat2 = np.random.randint(0,1000,size=(m,m2))
    #print(mat2)
    np.savetxt("OF_" + sys.argv[5],mat2,fmt='%i')
    mat2T = mat2.transpose();
    np.savetxt(sys.argv[5],mat2T,fmt='%i')

    matRes = np.matmul(mat,mat2)
    #print(matRes)
    #print(matRes.shape)
    if (matRes>LONG_LONG_MAX).sum() > 0:
        genMatrix()
    np.savetxt("matrixResult.txt",matRes,fmt='%i')

if __name__ == "__main__":
    genMatrix()
