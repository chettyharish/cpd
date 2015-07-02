import os


if __name__ == "__main__":
    for i in range(1,100):
        cmd = "time ./omp 1427 1553 " + str(i) + " >> tempomp "
        print(cmd)
        os.system(cmd)