import random
import sys
import time
import threading
import math





maxlen = False
num_threads = 32
threads = []

def issorted(data):
    for i in range(len(data) - 1):
        if data[i] > data[i + 1]:
            return False

    return True

def modder(tid, div, data, base, buckets):
#     print("Thread =  : " + str(tid))
    global maxlen , num_threads
    ele_per_blk = math.ceil(len(data) / num_threads)
    for i in range(tid * ele_per_blk, (tid + 1) * ele_per_blk):
        if(i >= len(data)):
            return
        temp = int(data[i] / div)
        buckets[temp % base].append((i, data[i]))
        if maxlen and temp > 0:
            maxlen = False
    return


def radixsort_parallel(data):
    base, pos, div = (10, -1, 1)
    global maxlen , num_threads

    while (maxlen == False):
        maxlen = True
        buckets = [list() for _ in range(base)]

        for tid in range(num_threads):
            t = threading.Thread(
                target=modder, args=(tid, div, data, base, buckets))
            threads.append(t)
            t.start()

        div *= base

        k = 0
        for i in range(base):
            for j in sorted(buckets[i]):
                data[k] = j[1]
                k += 1


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
    A = random.sample(range(0, 10**10), size)
    curr = time.time()
    print("FIRST : " + str(curr - start))
    if int(sys.argv[2]) == 0:
        print("RadixSort")
        radixsort(A)
    elif int(sys.argv[2]) == 1:
        print("RadixSort Parallel")
        radixsort_parallel(A)
    else:
        print("TimSort")
        sorted(A)
    print("END : " + str(time.time() - curr))
    
#     print(A)
    if(issorted(A)):
        print("Sorted Correctly")
    else:
        print("Error sorting")
