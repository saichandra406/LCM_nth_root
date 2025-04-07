#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_SIZE 3700  // Buffer space
#define MAX_LIMIT 1000000000  // Max prime power

// Macro for bit manipulation (as you have) [cite: 1]
#define GET_BIT(array, index) ((array[index >> 3] >> (index & 7)) & 1)

// Struct to store prime power and its logarithm
typedef struct {
    uint64_t p_power;
    double log_value;
} PrimePower;

typedef struct {
    PrimePower arr[MAX_SIZE];
    int size;
    int start_index;
} SortedArray;

// Binary search to find insert position (within [start_index, size))
int find_insert_position(SortedArray *s, int value) {
    int left = s->start_index;
    int right = s->size;

    while (left < right) {
        int mid = (left + right) / 2;
        if (s->arr[mid].p_power < value)
            left = mid + 1;
        else
            right = mid;
    }
    return left;
}

// Insert value while maintaining sorted order
void insert_sorted(SortedArray *s, uint64_t value, double log_p) {
    if (s->size >= MAX_SIZE) {
        fprintf(stderr, "Array overflow!\n");
        exit(1);
    }

    int pos = find_insert_position(s, value);
    memmove(&s->arr[pos + 1], &s->arr[pos], (s->size - pos) * sizeof(PrimePower));
    s->arr[pos].p_power = value;
    s->arr[pos].log_value = log_p;
    s->size++;
}

// check if number is in prime power array, return its log value
double has_primePower(SortedArray *s, uint64_t num) {
    if (s->arr[s->start_index].p_power == num) {
        double tmp = s->arr[s->start_index].log_value;
        s->start_index++;
        return tmp;
    }
    return 0.0;
}

// Generate prime powers and insert. Return log value of prime
double insert_prime_powers(SortedArray *s, uint64_t prime, uint64_t max_n) {
    uint64_t pows = prime * prime;
    double log_p = log((double) prime);
    while (pows <= max_n) {
        insert_sorted(s, pows, log_p);
        pows *= prime;
    }
    return log_p;
}

// Function to generate primes using the Sieve of Eratosthenes with bit array [cite: 1, 2, 3]
void generate_primes(uint8_t *is_prime, uint64_t limit) {
    // Initialize sieve (optimized - setting 0 and 1) [cite: 1]
    is_prime[0] = 0b11111100;

    for (uint64_t i = 2; i * i <= limit; i++) {
        if (GET_BIT(is_prime, i)) {
            for (uint64_t j = i * i; j <= limit; j += i) {
                is_prime[j >> 3] &= ~(1 << (j & 7));
            }

        }
    }
}

// Function to allocate memory and initialize it to all ones [cite: 3, 4, 5]
void* calloc_ones(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size;
    void* ptr = malloc(total_size);
    if (ptr != NULL) {
        memset(ptr, 0xFF, total_size); // Initialize to all ones (0xFF) [cite: 4, 5]
    }
    return ptr;
}

int main() {
    uint64_t max_n;

    printf("Enter the maximum value of n: ");
    scanf("%lu", &max_n);

    uint64_t sieve_len = (max_n >> 3) + 1;
    SortedArray s = { .size = 0, .start_index = 0 };

        // 1. Sieve of Eratosthenes
    uint8_t *sieve = (uint8_t *)calloc_ones(sieve_len, sizeof(uint8_t));
    if (sieve == NULL) {
        printf("Memory allocation failed for sieve.\n");
        return 1; // Indicate error
    }
    generate_primes(sieve, max_n);

    time_t start, end;
    time(&start);

    // Precompute logarithms
    //double ln_27 = log(2.7);
    //double ln_28 = log(2.8);
    //double ln_271 = log(2.71);
    //double ln_272 = log(2.72);
    double ln_2718 = log(2.718);
    double ln_2719 = log(2.719);
    uint64_t val = 0;

    double von_mangoldt_sum = 0.0;
    //for 1st byte of sieve as 0 and 1 are neither prime nor composite
    for (int n = 2; n < 8; n++) {
        if (GET_BIT(sieve, n)) { // n is prime
            von_mangoldt_sum += insert_prime_powers(&s, n, max_n);
        } else { // n is composite
            if(n == 4){
                von_mangoldt_sum += s.arr[0].log_value;
            s.start_index++;
            }
        }
        
        double nth_root = von_mangoldt_sum / (double) n;
        //printf("n=%d: %.12f\n",n, exp(nth_root));
    }

    for (uint64_t i=1; i < sieve_len; i++) {
        uint8_t byte_sieve = sieve[i] , b = 0;

        //if(byte_sieve == 0) continue;

        for(uint64_t j=0; j < 8; j++){
            double log_value = 0.0;
            b = (byte_sieve >> j) & 1; //bool true or false
            uint64_t num = (i*8) + j;
            if(num > max_n)
                break;
            if(b) // if is prime
                log_value = insert_prime_powers(&s, num, max_n);

            else{
                log_value = has_primePower(&s, num);
            }
            von_mangoldt_sum += log_value;
            double nth_root = von_mangoldt_sum / (double) num;

            if(!(ln_2718 < nth_root && nth_root < ln_2719)) val = num;

            //if(num == 36969509 || num == 36969510 || num == 36969511)
                //printf("n=%lu: %.6f, %.13f\n",num, exp(nth_root), nth_root);
        }
    }

    /*
    // Example: insert prime powers of first few primes
    int prime_count = sizeof(primes) / sizeof(primes[0]);

    for (int i = 0; i < prime_count; ++i) {
        insert_prime_powers(&s, primes[i]);


        // Access & maybe ignore current min
        printf("Min so far: %d\n", get_min(&s));
        ignore_min(&s);  // Optional, based on your logic

    }

    // Final sorted prime powers (starting from start_index)                                                                                                                            147,5         92%
    printf("\nFinal elements:\n");
    for (int i = s.start_index; i < s.size; ++i)
        printf("%d ", s.arr[i].p_power);
    printf("\n");

    printf("Total size: %d\n", s.size);
    */
    time(&end);
    free(sieve);

    printf("hundredths place = 1 from : %ld\n", val);
    printf("Elapsed time: %ld sec\n", end-start);
    return 0;
}
