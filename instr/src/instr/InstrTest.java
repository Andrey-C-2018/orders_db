package instr;

public class InstrTest {

	public static void main(String args[]) {
		
		try {
			Instr instr = new Instr();
			
			instr.evaluateInstr();
			instr.print();
		}
		catch(Exception e) {
			
			e.printStackTrace();
		}

	}
}
