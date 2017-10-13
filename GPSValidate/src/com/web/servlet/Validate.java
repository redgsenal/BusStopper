package com.web.servlet;

import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONException;
import org.json.JSONObject;

import com.app.utils.Log;

/**
 * Servlet implementation class Validate
 */
@WebServlet("/Validate")
public class Validate extends HttpServlet {
	private static final long serialVersionUID = 1L;

	/**
	 * @see HttpServlet#HttpServlet()
	 */
	public Validate() {
		super();
	}

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request,response);
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		String lat = request.getParameter("lat");
		String lon = request.getParameter("lon");
		response.setContentType("application/json");
		response.setCharacterEncoding("UTF-8");
		JSONObject jr = new JSONObject();
		try {			
			if ((lat == null || lat.isEmpty()) || (lon == null || lon.isEmpty())) {
				jr.put("success", false);
				jr.put("message", "empty coordinates");
				jr.put("valid", false);
				lat = "";
				lon = "";
			} else {
				jr.put("success", true);
				jr.put("message", "valid coordinates");
				jr.put("valid", true); // this is the response to check
			}		
		} catch (JSONException je) {
			Log.print(je.getLocalizedMessage());
			je.printStackTrace();
			jr.put("success", false);
			jr.put("valid", false);
			jr.put("message", je.getLocalizedMessage());		
		} catch (Exception e) {
			Log.print(e.getLocalizedMessage());
			e.printStackTrace();
			jr.put("success", false);
			jr.put("valid", false);
			jr.put("message", e.getLocalizedMessage());
		} finally {
			jr.put("lat", lat);
			jr.put("lon", lon);
			response.getWriter().write(jr.toString());			
		}		
	}
}
