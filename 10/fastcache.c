#define N 400

double mul1[N][N],
	mul2[N][N],
	res[N][N],
	tmp[N][N];

int main(void) {

int i = 0,
	j = 0,
	k = 0;

for (i = 0; i < N; ++i)
	for (j = 0; j < N; ++j)
     		tmp[i][j] = mul2[j][i];
  	for (i = 0; i < N; ++i)
    		for (j = 0; j < N; ++j)
      			for (k = 0; k < N; ++k)
       				res[i][j] += mul1[i][k] * tmp[j][k];
	return res[N-1][N-1];
}
