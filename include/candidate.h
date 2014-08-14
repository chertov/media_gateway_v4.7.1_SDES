/*============================================================================
 *Name        : candidate.cpp
 *Author      : Zhao Guoshuai
 *Version     :
 *Copyright   : Your copyright notice
 *Description : package the NiceCandidate into candidate class
 *============================================================================
 */

#ifndef CANDIDATE_H_
#define CANDIDATE_H_

#include <nice/agent.h>

class Candidate{
	private :
		NiceCandidate *candidate;

	public:
		Candidate();
		Candidate(const char str[]);
		Candidate(NiceCandidateType type);
		Candidate(NiceCandidateType type, NiceCandidateTransport tran, NiceAddress addr, guint32 priority, guint streamID, guint componentID, gchar foundation[]);
		~Candidate();
		Candidate &operator=(Candidate &other);

		NiceCandidate *getNiceCandidate(){
			return this->candidate;
		}

		void setCandidateProperties(NiceCandidateTransport tran, NiceAddress addr, guint32 priority, guint streamID, guint componentID, gchar foundation[]);
};

#endif /* CANDIDATE_H_ */
