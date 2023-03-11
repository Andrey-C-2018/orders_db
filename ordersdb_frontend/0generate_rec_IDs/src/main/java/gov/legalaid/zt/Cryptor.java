package gov.legalaid.zt;

import java.util.Base64;

public class Cryptor {

	public static void main(String[] args) {

		if (args.length != 1)
			System.err.println("Nothing to encrypt");
		else {
			Cryptor cryptor = new Cryptor();
			String encrypted = cryptor.encrypt(args[0]);
			System.out.println(encrypted);
			System.out.println(cryptor.decrypt(encrypted) + "<-");
		}
	}

	public Cryptor() {

		key = "bh386G4fw5tguM23e";
	}

	public String encrypt(String cler) {

		Base64.Encoder enc = Base64.getEncoder();

		StringBuilder crypted = new StringBuilder();
		for (int i = 0; i < cler.length(); i++) {
			int e = cler.charAt(i) ^ key.charAt(i);
			crypted.append((char)e);
		}

		return enc.encodeToString(crypted.toString().getBytes());
	}

	public String decrypt(String cipher) {

		Base64.Decoder dec = Base64.getDecoder();
		byte[] bytes = dec.decode(cipher);
		String cipher_data = new String(bytes);

		StringBuilder decrypted = new StringBuilder();
		for (int i = 0; i < cipher_data.length(); i++) {
			int e = cipher_data.charAt(i) ^ key.charAt(i);
			decrypted.append((char)e);
		}

		return decrypted.toString();
	}

	private final String key;
}
