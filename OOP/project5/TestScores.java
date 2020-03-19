package project05_1;

import java.util.Scanner;

public class TestScores {

	public static final int MAX_NUMBER_SCORES = 10;

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		double[] scores = new double[MAX_NUMBER_SCORES];
		int counting = 0;
		
		System.out.println("This program reads test scores and shows");
		System.out.println("how much each differs from the average.");
		System.out.println("Enter test scores:");
		
		counting = fillArray(scores);
		showDifference(scores, counting);
		
	}

	public static int fillArray(double[] scores)
	{
		System.out.println("Mark the end of the list with a negative number.");	
		Scanner keyboard = new Scanner(System.in);
		int count = 0;
		for(int i=0;i<MAX_NUMBER_SCORES;i++) {
			scores[i]=keyboard.nextDouble();
			if(scores[i]<0) {
				return count;
			}
			count++;
		}
		return count;
	}
	
	public static void showDifference(double[] scores,int counting)
	{
		double average = computeAverage(scores,counting);
		
		System.out.println("Average of the "+counting+" scores = " + average);
		System.out.println("The scores are: ");
		
		for(int i=0;i<counting;i++) {
			System.out.println(scores[i]+" differs from average by "+(scores[i]-average));
		}
	}
	
	public static double computeAverage(double[] scores,int counting)
	{	
		int i;
		double sum=0;
		for(i=0;i<counting;i++) {
			sum+=scores[i];
		}
		return sum/counting;
	}
}
