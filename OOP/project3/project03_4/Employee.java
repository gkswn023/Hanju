package project03_4;

public class Employee {

	private String name;
	private int age;
	private String position;
	private int salary;
	private int vacationDays;
	
	public Employee() {}
	
	public Employee(String name, int age) {
		this.name=name;
		this.age=age;
		this.position="Engineer";
		this.salary=15000;
		this.vacationDays=20;
	}
	
	public Employee(String name, int age, String position, int salary, int vacationDays) {
		this.name=name;
		this.age=age;
		this.position=position;
		this.salary=salary;
		this.vacationDays=vacationDays;
	}
	
	public void setSalary(int salary) {
		this.salary=salary;
	}
	
	public void outInfo() {
		System.out.println("name: "+this.name);
		System.out.println("age: "+this.age);
		System.out.println("position: "+this.position);
		System.out.println("salary: "+this.salary);
		System.out.println("vacationDays: "+this.vacationDays);
		System.out.println("\n");
	}
	
	public String vacation(int vacationDays) {
		if(this.vacationDays>=vacationDays) {
			this.vacationDays-=vacationDays;
		 	return "Possible\n";
		}
		else {
			return "Impossible\n";
		}
	}
}
