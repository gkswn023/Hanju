package project03_1;

import java.util.Scanner;

/**
 * Int ���� Ű���带 ���� �Է� ���� ��, �Է� ���� ���� ��� ������ ���ϴ��� ���ϴ� ���α׷��� �����϶�.
 * 
 * ��û����
 * 		- Scanner�� ����Ͽ� Ű����κ��� Int�� ���� n�� ���� �Է� ���� ��.
 * 		- if ���� ����Ͽ� �Է� ���� n�� ���� �Ʒ��� ���Ǵ�� �����Ͽ� Consoleâ�� ����� ��.
 * 		- n�� ���� ����
 * 			* n�� 0 �̸��� ���, 'n is less than 0' ������ ���.
 * 			* n�� 0 ���� ũ�� 100 �̸��� ���, 'n is greater than or equal to 0 and less than 100' ������ ���.
 * 			* n�� 100���� Ŭ ���, 'n is greater than or equal to 100' ������ ���.
 */

public class BranchStmt {

	public static void main(String[] args) {		
		System.out.print("n = ? ");
		
		/* �л� �ڵ� �ۼ� �κ� - ���̿� �ڵ带 �ۼ��Ͻÿ�. */
		Scanner keyboard = new Scanner(System.in);
		
		int n = keyboard.nextInt();
		
		if(n<0) System.out.println("n is less than 0");
		
		else if(n>=0&&n<100) {
			System.out.println("n is greater than or equal to 0 and less than 100");
		}
		
		else {
			System.out.println("n is greater than or equal to 100");
		}
		/* �л� �ڵ� �ۼ� �κ� - ���̿� �ڵ带 �ۼ��Ͻÿ�. */
	}
}
