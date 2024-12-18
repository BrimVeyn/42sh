import React, { useEffect, useState } from 'react';
import Category from './Category'
import Resume from './Resume'
import logo from './logo.svg';

import './App.css';
import './input.css'

function App() {
	const [data, setData] = useState(null);

	useEffect(() => {
		fetch('http://localhost:4000/api/logs')
			.then((response) => {
				if (!response.ok) {
					throw new Error(`HTTP error! status: ${response.status}`);
				}
				return response.json(); // Parse the JSON response
			})
			.then((json) => {
				setData(json); // Update state with the fetched JSON data
			})
			.catch((error) => {
				console.error('Error fetching the JSON file:', error);
			});
	}, []);
	// console.log(data);

	const [resumeContent, setResumeContent] = useState(null);

	const injectButton = (category) => {
		setResumeContent(<Resume tests={category.tests} title={category.category_name} />);
	};

	return (
		<div>
			<header className="flex App-header">
				<div className="flex flex-wrap w-1/3 m-2 p-2 categories">
					{data ? (
						data.categories.map((category, index) => (
								<Category 
									key={index}
									name={category.category_name} 
									testsInfo={{ passed: category.passed_tests, total: category.tests.length}}
									inject={() => injectButton(category)}
								/>
						))
					) : (
							<p> Loading test results </p>
						)}
				</div>

				<div className="w-2/3 flex flex-col justify-start items-center">
					{resumeContent}
				</div>

			</header>
		</div>
	);
}

export default App;
