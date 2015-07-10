import random
import sys
import time


def issorted(data):
    for i in range(len(data) - 1):
        if data[i] > data[i + 1]:
            return False

    return True


def radixsort(data):
    base, maxlen, pos, div = (10, False, -1, 1)

    while (maxlen == False):
        maxlen = True
        buckets = [list() for _ in range(base)]

# Parallelizable completely except buckets
        for i in data:
            temp = int(i / div)
            buckets[temp % base].append(i)
            if maxlen and temp > 0:
                maxlen = False

        div *= base

# Not parallelizable directly
        k = 0
        for i in range(base):
            for j in buckets[i]:
                data[k] = j
                k += 1


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("More args!")
        exit(0)

#     A = [18, 5, 100, 3, 1, 19, 6, 0, 7, 4, 2]
    start = time.time()
    size = int(float(sys.argv[1]))
    print(8 * 10e9 / size)
    A = random.sample(range(0,10**10) , size)
    curr = time.time()
    print("FIRST : " + str(curr - start))
    if int(sys.argv[2]) == 0:
        print("RadixSort")
        radixsort(A)
    else:
        print("TimSort")
        sorted(A)
    print("END : " + str(time.time() - curr))

    if(issorted(A)):
        print("Sorted Correctly")
    else:
        print("Error sorting")
