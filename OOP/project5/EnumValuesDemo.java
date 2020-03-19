package project05_2;

import java.util.Scanner;

public class EnumValuesDemo {

	enum Dates {MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY};
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		Scanner key = new Scanner(System.in);
		Dates[] dates = Dates.values();
		int sum = 0;
		for(int i=0;i<5;i++) {
			System.out.println("Input work time (" + dates[i] + ") : ");
			sum = sum + key.nextInt();
			}
		System.out.println("Total Time : "+sum);
		}
}
