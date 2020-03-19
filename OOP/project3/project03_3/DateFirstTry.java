package project03_3;

public class DateFirstTry {
	
	public String month="December";
	public int day=30;
	
	public void makeItNewYears() {
		this.month="January";
		this.day=1;
	}
	
	public String yellIfNewYear() {
		if(this.month.equals("January")&&this.day==1) {
			return "Happy New Year!";
		}
		else {
			return "Not New Year's Day.";
		}
	}
}


