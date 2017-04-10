package iot;

import java.io.ObjectOutputStream;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;

import javax.swing.ImageIcon;

import com.mysql.jdbc.Blob;



public class Main {

	public static void main(String[] args) {
			     String dbURL = "jdbc:mysql://localhost:3306/company";
			     String dbUser = "root";
			     String dbPass = "1234";

			    //naive way to obtain a connection to database
			    //this MUST be improved, shown for 
			 //   private Connection getConnection() {
			    //    Connection con = null;
			        try {
			        	Class.forName("com.mysql.jdbc.Driver");
						Connection con=DriverManager.getConnection(dbURL,dbUser,dbPass);
						PreparedStatement ps=con.prepareStatement("insert into iot(name,photo)"+"values(?,?)");
				//		ps.con.prepareStatement("insert into image(name,photo)"+"values(?,?)");
						ps.setString(1,"hghs");
						Blob blob=(Blob) con.createBlob();
						ImageIcon ii=new ImageIcon("urvashi.jpg");
						ObjectOutputStream oos;
						oos=new ObjectOutputStream(blob.setBinaryStream(1));
						oos.writeObject(ii);
						oos.close();
						ps.setBlob(2, blob);
						ps.execute();
						blob.free();
						ps.close();
						// DriverManager.registerDriver(new com.mysql.jdbc.Driver());
			            //conn = DriverManager.getConnection(dbURL, dbUser, dbPass);
			        } catch (Exception e) {
			            //wrapping any exception and rethrowing it
			            //inside a RuntimeException
			            //so the method is silent to exceptions
			            throw new RuntimeException("Failed to obtain database connection.", e);
			        }
			        //return con;
			    }


			

		

	}


