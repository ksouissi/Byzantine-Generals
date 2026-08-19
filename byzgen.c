/* see byzgen.txt for proof.

Simulation:

x0, ... x3n: 3n+1 nodes
u[i, j] = what node xj says to xi, if xj trustful then == j
K a fixed value

val(k, i(j-1), ... i0) {
	if (j-1 == 0 || erratic(xi(j-1)))
		return u[k, i(j-1)]
	(* !erratic(xi(j-1)) && (xi(j-2), ... xi0) != () *)
	return val(i(j-1), i(j-2), ... i0)	(* say what xi(j-1) says that xi(j-2) says that  ... xi0 says *)
}

decide(k, i(j-1), ... i0, m) {	(* xk trustful && j > 0 *)
	v[k] = val(k, i(j-1), ... i0)
	if (m == 0)
		return v[k]
	foreach (p in { q | 0 <= q < 3n+1 && q not in { k, i(j-1), ... i0 } })
		v[p] = decide(k, p, i(j-1), ... i0, m-1)
	return value in v that occurs most often, if none then K
}

decide(i, j, n) == what xi decides that xj says */

/* this is very slow indeed, a general rule: doubt (calculation) is inefficient */

#define BASE	6	/* 3*BASE+1 nodes: 1, 2, ... 3*BASE+1 */
#define ERR(i)	(err & (1 << (i)-1))

int err = 0x1010f;	/* example erratic node-map: 1-4, 9 and 17 */

int val(int k, int l[])	/* l: zero-terminated string of node-indices
							l[0] != 0 */
{
	if (!l[1] || ERR(l[0]))
		return ((l[0] ^ -ERR(l[0]) >> 31 & k)-1)%(3*BASE+1)+1;	/* == l[0] if trustful, a function of l[0] and k in [1, 3*BASE+1] otherwise */
	return val(l[0], &l[1]);	/* l[0] trustful: repeat */
}

int loc(int x)
{
	int i = 0;

	x--;
	while (x) {
		i++;
		x &= x-1;
	}
	return i;
}

int decide(int k, int l[], int m)
{
	if (m)
	{
		int i, j, q, L[BASE+2], c[3*BASE+2];	/* 0 = no majority */

		q = (1 << 3*BASE+1)-1;	/* map of remaining nodes */
		i = BASE+1-m;
		L[1+i--] = 0;	/* terminator */
		do {
			q ^= 1 << l[i]-1;
			L[1+i] = l[i];
		} while (i--);
		q ^= 1 << k-1;

		i = 3*BASE+1;	/* clear c */
		do {
			c[i] = 0;
		} while (i--);

		c[val(k, l)]++;	/* direct */
		do {	/* indirect */
			L[0] = loc(q & -q)+1;
			j = decide(k, L, m-1);
			c[j]++;
			if (c[j] > (3*BASE+1-(BASE+1-m))/2)	/* majority */
				return j;
		} while (q &= q-1);

		return 0;	/* no majority */
	}
	/* !m */
	return val(k, l);
}

#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
	if (argc >= 3) {
		int l[] = { atoi(argv[2]), 0 };
		printf("%d", decide(atoi(argv[1]), l, BASE));
	} else
		printf("Usage: byzgen.exe <trustful inquisitor> <suspect>\nthe former in [5-8, 10-16, 18-19], the latter in [1, 19]");
	return 0;
}
