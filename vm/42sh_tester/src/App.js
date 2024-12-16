import React, { useEffect, useState } from 'react';
import Category from './Category'
import Resume from './Resume'
import logo from './logo.svg';

import './App.css';
import './input.css'

function App() {
	const [data, setData] = useState(null);

	useEffect(() => {
		// Fetch the JSON file from the public folder
		fetch('/log.json')
			.then((response) => {
				if (!response.ok) {
					throw new Error(`HTTP error! status: ${response.status}`);
				}
				return response.json(); // Parse JSON
			})
			.then((json) => {
				setData(json); // Update state with parsed data
			})
			.catch((error) => {
				console.error('Error fetching the JSON file:', error);
			});
	}, []); // Empty dependency array to run only once on component mount

	// console.log(data);

	const [resumeContent, setResumeContent] = useState(null);

	const injectButton = (tests) => {
		setResumeContent(<Resume tests={tests} />);
	};

	return (
		<div>
			<header className="flex App-header">
				<div className="w-1/3 categories">
					{data ? (
						data.categories.map((category, index) => (
							<Category 
								name={category.category_name} 
								inject={() => injectButton(category.tests)}
							/>
						))
					) : (
							<p> Loading test results </p>
						)}
				</div>

				<div className="w-2/3 flex flex-col justify-start items-center">
					{resumeContent ? (
						resumeContent
					) : (
						<p> Nothing </p>
					)}
				</div>

			</header>
		</div>
	);
}

export default App;
