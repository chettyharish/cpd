import random
import sys


def radixsort(aList):
    RADIX = 10
    maxLength = False
    tmp, placement = -1, 1

    while not maxLength:
        maxLength = True
        # declare and initialize buckets
        buckets = [list() for _ in range(RADIX)]

        # split aList between lists
        for i in aList:
            tmp = int(i / placement)
            buckets[tmp % RADIX].append(i)
            if maxLength and tmp > 0:
                maxLength = False

        # empty lists into aList array
        a = 0
        for b in range(RADIX):
            buck = buckets[b]
            for i in buck:
                aList[a] = i
                a += 1

        # move to next digit
        placement *= RADIX

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("More args!")
        exit(0)

#     A = [18, 5, 100, 3, 1, 19, 6, 0, 7, 4, 2]
    A = random.sample(range(0,10**10) , 10**6) 
    if int(sys.argv[1]) == 0:
        print("RadixSort")
        radixsort(A)
    else:
        print("TimSort")
        sorted(A)
#     print(A)
