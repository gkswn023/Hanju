package project04_2;

public class City {
	
	private String name;
	private double lat;
	private double lon;
	
	public City() {}
	
	public City(String name, double lat, double lon) {
		this.name=name;
		this.lat=lat;
		this.lon=lon;
	}
	
	public City(String name) {
		this.name=name;
		this.lat=Math.random()*360;
		this.lon=Math.random()*360;
	}
	
	public boolean equals(City city) {
		if(name.equals(city.name) && lat==city.lat && lon==city.lon)
			return true;
		else {
			return false;
		}
	}
	
	public String toString() {
		return this.name+", "+this.lat+", "+this.lon;
	}
	
	public static double cityDistance(City c1, City c2) {
		double x,y,z;
		x=Math.pow(c1.lon-c2.lon,2);
		y=Math.pow(c1.lat-c2.lat, 2);
		z=Math.sqrt(x+y);
		
		return z;
	}
}
