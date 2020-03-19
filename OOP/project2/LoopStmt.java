package project03_2;

/**
 * Int형 변수 두 개(n, m)를 사용하여, 곱셈한 결과를 Console창에 출력하라.
 * 
 * 요청사항
 * 		- n과 m은 1부터 5까지이다.
 * 		- 출력할 문구는 'n multiplied by m = n*m' 이다. (n, m, n*m에는 각각의 값이 출력되야한다.) 
 */

public class LoopStmt {

	public static void main(String[] args) {
		/* 학생 코드 작성 부분 - 사이에 코드를 작성하시오. */
		int n,m;
		
		for(n=1;n<6;n++)
		{
			for(m=1;m<6;m++) {
				System.out.printf("%d multiplied by %d = %d\n",n,m,n*m);
			}
		}
		/* 학생 코드 작성 부분 - 사이에 코드를 작성하시오. */
	}

}
