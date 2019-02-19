package instr;

public class InstrException extends Exception {

	private static final long serialVersionUID = 1L;
	private int code; 

	public static final int E_NEXT_INSTR = 1;
	
	public InstrException(int code, String err_msg) {
		
		super(err_msg);
		this.code = code;
	}
	
	public final int getErrorCode() {
		
		return code;
	}
}
