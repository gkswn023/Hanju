package project03_4;

public class EmployeeManager {

	public static void main(String[] args) {
		// TODO Auto-generated method stub

		Employee employee1 = new Employee("James Wright", 42, "Manager", 20000, 20);
		Employee employee2 = new Employee("Peter Coolidge", 32, "Assistant Manager", 12000, 7);
		Employee employee3 = new Employee("Jimmie McGill", 38);
		Employee employee4 = new Employee("Chuck McGill", 40);
		
		employee1.setSalary(21000);
		
		employee1.outInfo();
		employee2.outInfo();
		
		System.out.println(employee3.vacation(10));
		System.out.println(employee2.vacation(10));
		
		employee3.outInfo();
		
	}

}
