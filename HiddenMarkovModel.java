                                                                     
                                                                     
                                                                     
                                             
import java.awt.SystemColor;
import java.util.LinkedList;
import java.util.List;
import java.util.Random;


public class HiddenMarkovModel {
	
	public static boolean debug = true;
	
	public static void main(String args[]) {
		HiddenMarkovModel HMM = new HiddenMarkovModel();
		HMM.test();	
	}
	
	/**
	 * Tests the EM and Gibbs sampler for HMM.  If all works well, the EM should have strictly decreasing NLL and the Gibbs should be mostly decreasing to some stable point.
	 */
	public void test() {
		Random generator = new Random(1);
		
		int numStates = 5;
		double tranP[][] = new double[numStates][numStates];
		
		for (int i = 0; i < numStates; i++) {
			for (int j = 0; j < numStates; j++) {
				tranP[i][j] = generator.nextDouble();
				if (i == j) tranP[i][j] += 10;
			}
			tranP[i] = vecNomalize(tranP[i]);
		}
		
		double mu[] = new double[numStates];
		for (int i = 0; i < numStates; i++) {
			mu[i] = generator.nextGaussian() * 5;
		}
		
		double sigma[] = new double[numStates];
		for (int i = 0; i < numStates; i++) {
			sigma[i] = (generator.nextDouble() + 0.2) * 3;
		}
		
		double initialP[] = new double[numStates];
		for (int i = 0; i < numStates; i++) {
			initialP[i] = generator.nextDouble();
		}
		initialP = vecNomalize(initialP);
		int length = 10000;
		double obsStates[] = genMarkovData(mu, sigma, tranP, initialP, numStates, length);
		
		System.out.println("Testing EM");
		inferEM(obsStates, numStates-1, 1000, 0.1);
		inferEM(obsStates, numStates, 1000, 0.1);
		inferEM(obsStates, numStates+10, 1000, 0.1);
		System.out.println("Testing Gibbs");
//		inferGibbs(obsStates, numStates, 1000, 0.1);

	}
	
	/**
	 * Generates random data from an HMM with gaussian emissions.
	 * @param mus The means of the gaussian emissions.
	 * @param sigmas The standard deviations of the gaussian emissions.
	 * @param tranProb A matrix of transition probabilities
	 * @param initialProb An array of probabilities for the initial state.
	 * @param numStates The number of states
	 * @param length The length of the sequence to generate.
	 * @return The observed states from the generate HMM.
	 */
	public double[] genMarkovData(double[] mus, double[] sigmas, double[][] tranProb, double[] initialProb, int numStates, int length) {
		int hiddenStates[] = new int[length];
		hiddenStates[0] = randSample(initialProb);
		for (int i = 1; i < length; i++) {
			double tran[] = tranProb[hiddenStates[i-1]];
			hiddenStates[i] = randSample(tran);
		}
		
		Random generator = new Random();
		double obsStates[] = new double[length];
		for (int i = 0; i < length; i++) {
			obsStates[i] = (generator.nextGaussian()  + mus[hiddenStates[i]]) * sigmas[hiddenStates[i]];
		}
		
		return obsStates;		
	}
	
	/**
	 * Takes a random sample from a categorical distribution
	 * @param probs An array of probabilities that sums to one where probs[i] = probability that i gets returned
	 * @return A weighted random value from the distribution probs.
	 */
	private int randSample(double[] probs) {
		double rand = Math.random();
		for (int i = 0; i < probs.length; i++) {
			if (probs[i] > rand) return i;
			else rand -= probs[i];				
		}
		return probs.length - 1;
	}
	
	/**
	 * Calculates an unnormalized normal pdf.
	 * @param x Value to calculate probability of.
	 * @param mu Mean of the gaussian.
	 * @param sigma Standard deviation of the Gaussian.
	 * @return Unnormalized probability of x from a gaussian with mean mu and standard deviation mu.
	 */
	private double normPdf(double x, double mu, double sigma) {
		return Math.exp(-square((x - mu)/ sigma) / 2) / sigma / Math.sqrt(2*Math.PI);
	}
	
	/**
	 * Quickly calculates the square of double.
	 */
	private final double square(double x){
		return x*x;
	}
	
	/**
	 * Calculates the L2 norm of a given vector.
	 */
	private final double norm(double[] x) {
		double temp = 0;
		for (int i = 0; i < x.length; i++) {
			temp += x[i] * x[i];
		}
		return Math.sqrt(temp);
	}
	
	/**
	 * Calculates the sum of a given vector.
	 */
	private final double sum(double[] x) {
		double temp = 0;
		for (int i = 0; i < x.length; i++) {
			temp += x[i] ;
		}
		return temp;
	}
	
	/**
	 * Calculates the sum of a given vector.
	 */
	private final double[] vecNomalize(double[] x) {
		double sum = sum(x);
		double toReturn[] = new double[x.length];
		for (int i = 0; i < x.length; i++) {
			toReturn[i] = x[i] / sum;
		}
		return toReturn;
	}
	
	/**
	 * Divides all the elements of a given vector x, by a value divisor.
	 */
	private final double[] vecDivide(double[] x, double divisor) {
		double toReturn[] = new double[x.length];
		for (int i = 0; i < x.length; i++) {
			toReturn[i] = x[i] / divisor;
		}
		return toReturn;
	}
	
	/**
	 * Uses a Gibbs Sampler to infer the states, transition, and emission probabilities for a HMM where the observations are assumed to come from a univariate gaussian distribution.
	 * @param observed A vector of observed data assumed to be from a univariate normal distribution.
	 * @param numStates The number of hidden states in the HMM.
	 * @param maxIters Maximum number of times to iterate the EM algorithm
	 * @param cutOff The required relative change in probability before stopping the algorithm.
	 * @return
	 */
	public double[] inferGibbs(double[] observed, int numStates, int maxIters, double cutOff) {
		Random generator = new Random();

		int length = observed.length;
		
		double[] mu = kmeans(observed, numStates);
		double[] sigma = new double[numStates];
		double[] pi = new double[numStates];
		double[][] tran = new double[numStates][numStates];
		
		int[] postStates = new int[length]; 
		
		// Randomly initializes the parameters.
		for (int i = 0; i < numStates; i++) {
			pi[i] = generator.nextDouble();
			sigma[i] = 1;
			for (int j = 0; j < numStates; j++) {
				tran[i][j] = generator.nextDouble();
			}
			tran[i] = vecNomalize(tran[i]);
		}
		pi = vecNomalize(pi);
		double postProb[][] = new double[0][0];
		double prevProb = Double.MAX_VALUE;
		double curProb = Double.MIN_NORMAL;
		int curIter = 0;
		
		while (curIter < maxIters && Math.abs(prevProb - curProb) > cutOff) {	
			curIter++;
			// Call the forwards backwards algorithm to get the posterior probability of the states and transitions.
			Object[] fb = 
					forwardsBackwards(observed, mu, sigma, tran, pi, numStates);
			postProb = (double[][]) fb[0];
			
			// Randomly sample hidden states.
			for (int i = 0; i < length; i++) {
				postStates[i] = randSample(postProb[i]);
			}
			
			// Sample Transition matrix.
			
			for (int i = 0; i < numStates; i++) {
				for (int j = 0; j < numStates; j++) {
					tran[i][j] = 1;
				}
			}
			
			for (int i = 0; i < length-1; i++) {
				tran[postStates[i]][postStates[i+1]]++;
			}			
			for (int i = 0; i < numStates; i++) {
				tran[i] = vecNomalize(tran[i]);
			}
			
			// Maximizes the initial probability vector
			pi = vecNomalize(postProb[0]);			
			
			// Maximizes the mean parameter for the gaussian distributions.
			double[] sums = new double[numStates];
			for (int i = 0; i < numStates; i++) {
				mu[i] = 0;
				sigma[i] = 0;
				sums[i] = 1;
			}
			
			
			for (int i = 0; i < length; i++) {
				int curState = postStates[i];
				mu[curState] += observed[i];
				sums[curState]++;
			}
			
			for (int i = 0; i < numStates; i++)
				mu[i] /= sums[i];
			
			for (int i = 0; i < length; i++) {
				int curState = postStates[i];
				sigma[curState] += square(observed[i] - mu[curState]);;
			}
			
			for (int i = 0; i < numStates; i++) {
				sigma[i] /= sums[i];
				sigma[i] = Math.sqrt(sigma[i]);
			}			
			
			prevProb = curProb;
			curProb = dataProb(observed, mu, sigma, tran, pi, numStates);
			System.out.println("Step:" + curIter + ", NLL = " + curProb);
		}		
		
		
		for (int i = 0; i < length; i++) {
			System.out.print(maxIndex(postProb[i]));
		}
		System.out.println();
		
		return null;
	}
	
	
	/**
	 * Uses an EM algorithm to infer the states, transition, and emission probabilities for a HMM where the observations are assumed to come from a univariate gaussian distribution.
	 * @param observed A vector of observed data assumed to be from a univariate normal distribution.
	 * @param numStates The number of hidden states in the HMM.
	 * @param maxIters Maximum number of times to iterate the EM algorithm
	 * @param cutOff The required relative change in probability before stopping the algorithm.
	 * @return
	 */
	public double[] inferEM(double[] observed, int numStates, int maxIters, double cutOff) {
		Random generator = new Random();

		int length = observed.length;
		
		double[] mu = kmeans(observed, numStates);
		double[] sigma = new double[numStates];
		double[] pi = new double[numStates];
		double[][] tran = new double[numStates][numStates];
		
		// Randomly initializes the parameters.
		for (int i = 0; i < numStates; i++) {
			sigma[i] = 1;
			pi[i] = generator.nextDouble();
			for (int j = 0; j < numStates; j++) {
				tran[i][j] = generator.nextDouble();
			}
			tran[i] = vecNomalize(tran[i]);
		}
		pi = vecNomalize(pi);
		double postProb[][] = new double[0][0];
		
		double prevProb = Double.MAX_VALUE;
		double curProb = Double.MIN_NORMAL;
		int curIter = 0;
		
		while (curIter < maxIters && Math.abs(prevProb - curProb) > cutOff) {
			curIter++; 
			
			// Call the forwards backwards algorithm to get the posterior probability of the states and transitions.
			Object[] fb = 
					forwardsBackwards(observed, mu, sigma, tran, pi, numStates);
			postProb = (double[][]) fb[0];
			tran = (double[][]) fb[1];

			// Maximizes the initial probability vector
			for (int curState = 0; curState < numStates; curState++) {
				pi[curState] = postProb[0][curState];
			}
			pi = vecNomalize(pi);
			
			// Maximizes the mean and standard deviation parameters for the gaussian distributions.
			for (int state = 0; state < numStates; state++) {
				double sum = 0.0;
				mu[state] = 0;
				sigma[state] = 0;
				for (int j = 0; j < length; j++) {
					mu[state] += postProb[j][state] * observed[j];
					sum += postProb[j][state];
				}
				mu[state] /= sum;
				for (int j = 0; j < length; j++) {
					sigma[state] += postProb[j][state] * square(observed[j] - mu[state]);
				}
				sigma[state] /= sum;
				sigma[state] = Math.sqrt(sigma[state]);
			}			
			
			prevProb = curProb;
			curProb = dataProb(observed, mu, sigma, tran, pi, numStates);
			System.out.println("Step:" + curIter + ", NLL = " + curProb);
		}
		
		return null;
	}
	
	/**
	 * Calculates the negative log likelihood of the observed HMM data with the given parameters for a univariate gaussian emission.
	 * @param observed An array of the observed data.
	 * @param mu An array of the mean values of the emission gaussians.
	 * @param tranProb A 2D array where tranProb[i][j] = probability of transitioning from state i to state j.
	 * @param initialProb An array containing the initial probability of being in a given state
	 * @param numStates The number of unique states in the Markov Model
	 * @return The negative log likelihood of the data.
	 */
	public double dataProb(double[] observed, double[] mu, double[] sigma, double[][] tranProb, double[] initialProb, int numStates) {
		int length = observed.length;
		double cumProb[][] = new double[length][numStates];
		double scaling[] = new double[length];
		
		double curObs = observed[0];
		for (int state = 0; state < numStates; state++) {
			cumProb[0][state] = normPdf(curObs, mu[state], 1) * initialProb[state];
		}
		scaling[0] = Math.log(sum(cumProb[0]));
		cumProb[0] = vecNomalize(cumProb[0]);
		
		for (int t = 1; t < length; t++) {
			curObs = observed[t];
			for (int prevState = 0; prevState < numStates; prevState++) {
				for (int curState = 0; curState < numStates; curState++) {
					cumProb[t][curState] += cumProb[t-1][prevState] * tranProb[prevState][curState] * normPdf(curObs, mu[curState], sigma[curState]);
				}
			}
			scaling[t] = Math.log(sum(cumProb[t]));
			cumProb[t] = vecNomalize(cumProb[t]);
		}
		
		return - (sum(scaling) + Math.log(sum(cumProb[length-1])));
	}
	
	/**
	 * Uses the forwards backwards algorithm to find the posterior state probabilities of a HMM with gaussian emissions
	 * @param observed An array containing the observed data from the HMM
	 * @param mus An array containing the mean values of the emissions gaussians.
	 * @param sigmas An array containing the standard deviation of the emissions gaussians.
	 * @param tranProb A 2D array where tranProb[i][j] = probability of transitioning from state i to state j.
	 * @param initialProb An array containing the initial probability of being in a given state
	 * @param numStates The number of unique states in the Markov Model
	 * @return An object array where the first element is a 2D array where element [i][j]=probability that hidden state i is in state j
	 * and the second element is a 2D matrix for the posterior probability of the transition matrix.
	 */
	public Object[] forwardsBackwards(double[] observed, double[] mus, double[] sigmas, double[][] tranProb, double[] initialProb, int numStates) {
		int length = observed.length;
		double scaling[] = new double[length];
		
		// PART 1: The forwards algorithm
		double[][] alpha = new double[length][numStates];
		double curObs = observed[0];
		
		// Calculates the probabilities of the first state
		for (int initialState = 0; initialState < numStates; initialState++) {
			alpha[0][initialState] = normPdf(curObs, mus[initialState], sigmas[initialState]) * initialProb[initialState];
		}
		
		// Normalizes the probabilities to sum to one
		scaling[0] = sum(alpha[0]);
		alpha[0] = vecNomalize(alpha[0]);
		
		// Iterate over each state, finding probability given previous states
		for (int t = 1; t < length; t++) {
			curObs = observed[t];
			// Finds the probability of ending in state j
			for (int curState = 0; curState < numStates; curState++) {
				alpha[t][curState] = 0;
				// Adds up contribution of ending in j if you started in k
				for (int prevState = 0; prevState < numStates; prevState++) {
					alpha[t][curState] += alpha[t-1][prevState]*tranProb[prevState][curState];
				}
				alpha[t][curState] *= normPdf(curObs, mus[curState], sigmas[curState]);
			}
			// Normalizes the probabilities to sum to one
			scaling[t] = sum(alpha[t]);
			alpha[t] = vecNomalize(alpha[t]);
		}
		
		// PART 2: The backwards algorithm
		double[][] beta = new double[length][numStates];
		
		// The last state always has probability 1 since there's no data after it
		for (int finalState = 0; finalState < numStates; finalState++) {
			beta[length - 1][finalState] = 1;
		}
		beta[length - 1] = vecDivide(beta[length - 1], scaling[length - 1]);
		
		for (int t = length - 2; t >= 0; t--) {
			curObs = observed[t + 1];
			// Finds the probability of coming from state j
			for (int curState = 0; curState < numStates; curState++) {
				beta[t][curState] = 0;
				// Adds up contribution of coming from j to each next k
				for (int nextState = 0; nextState < numStates; nextState++) {
					beta[t][curState] += beta[t+1][nextState]*tranProb[curState][nextState] * normPdf(curObs, mus[nextState], sigmas[nextState]);
				}
			}
			// Normalizes the probabilities relative to the alphas
			beta[t] = vecDivide(beta[t], scaling[t]);
		}
		
		// PART 3: Calculates the posterior probability of being in a given state using forwards and backwards info.
		double postProb[][] = new double[length][numStates];
		for (int t = 0; t < length; t++) {
			for (int curState = 0; curState < numStates; curState++) {
				postProb[t][curState] = alpha[t][curState] * beta[t][curState];
			}
			postProb[t] = vecNomalize(postProb[t]);
		}
		
		
		// zeta(i, j) = prob of going from state i to j
		double zeta[][] = new double[numStates][numStates];
		for (int curState = 0; curState < numStates; curState++) {
			for (int nextState = 0; nextState < numStates; nextState++) {
				for (int t = 0; t < length - 1; t++) {
					zeta[curState][nextState] += alpha[t][curState] * normPdf(observed[t+1], mus[nextState], sigmas[nextState]) * beta[t+1][nextState] * tranProb[curState][nextState];
				}
			}
			zeta[curState] = vecNomalize(zeta[curState]);
		}
		
		Object toReturn[] = new Object[2];
		toReturn[0] = postProb;
		toReturn[1] = zeta;
				
		return toReturn;
	}
	
	private int maxIndex(double[] x) {
		double maxValue = x[0];
		int maxIndex = 0;
		for (int i = 1; i < x.length; i++) {
			if (x[i] > maxValue) {
				maxValue = x[i];
				maxIndex = i;
			}
		}
		return maxIndex;
	}
	
	/**
	 * Uses the k-means algorithm to find k clusters in an array of data
	 * @param data Array to find clusters in
	 * @param k Number of clusters to find
	 * @return An array with the centers of the k clusters.
	 */
	private double[] kmeans(double[] data, int k) {
		double[] centers = new double[k];
		List<Double>[] partedData = new LinkedList[k];
		Random gen = new Random(1);
		
		for (int i = 0; i < k; i++) {
			centers[i] = data[gen.nextInt(data.length)];
		}
		
		for (int iter = 0; iter < 10; iter++) {
			// Reset clustered data
			for (int i = 0; i < k; i ++) {
				partedData[i] = new LinkedList<Double>();
			}
			
			
			// Assign each data to a cluster
			for (int i = 0; i < data.length; i++) {
				int minCluster = 0;
				double curData = data[i];
				double minDist = Math.abs(centers[0] - curData);
				for (int j = 1; j < k; j++) {
					if (Math.abs(centers[j] - curData) < minDist) {
						minDist = Math.abs(centers[j] - curData);
						minCluster = j;
					}
				}
				partedData[minCluster].add(curData);
			}
			
			// Find the new cluster centers/
			for (int i = 0; i < k; i++) {
				double cumSum = 0;
				for (int j = 0; j < partedData[i].size(); j++)
					cumSum += partedData[i].get(j);
				centers[i] = cumSum / partedData[i].size(); 
			}			
		}				
		return centers;		
	}
	
	public void printVec(double[] x) {
		for (int i = 0; i < x.length; i++) {
			System.out.print(x[i] + " ");
		}
		System.out.println();
	}
}
