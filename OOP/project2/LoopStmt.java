package project03_2;

/**
 * Int�� ���� �� ��(n, m)�� ����Ͽ�, ������ ����� Consoleâ�� ����϶�.
 * 
 * ��û����
 * 		- n�� m�� 1���� 5�����̴�.
 * 		- ����� ������ 'n multiplied by m = n*m' �̴�. (n, m, n*m���� ������ ���� ��µǾ��Ѵ�.) 
 */

public class LoopStmt {

	public static void main(String[] args) {
		/* �л� �ڵ� �ۼ� �κ� - ���̿� �ڵ带 �ۼ��Ͻÿ�. */
		int n,m;
		
		for(n=1;n<6;n++)
		{
			for(m=1;m<6;m++) {
				System.out.printf("%d multiplied by %d = %d\n",n,m,n*m);
			}
		}
		/* �л� �ڵ� �ۼ� �κ� - ���̿� �ڵ带 �ۼ��Ͻÿ�. */
	}

}
